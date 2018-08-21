#include "ik/callbacks.h"
#include "ik/constraint.h"
#include "ik/effector.h"
#include "ik/log.h"
#include "ik/memory.h"
#include "ik/node.h"
#include "ik/pole.h"
#include "ik/quat.h"
#include "ik/vec3.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* ------------------------------------------------------------------------- */
struct ik_node_t*
ik_node_create(uint32_t guid)
{
    struct ik_node_t* node = MALLOC(sizeof *node);
    if (node == NULL)
    {
        ik_log_fatal("Failed to allocate node: Ran out of memory");
        return NULL;
    }
    ik_node_construct(node, guid);

    return node;
}

/* ------------------------------------------------------------------------- */
ikret_t
ik_node_construct(struct ik_node_t* node, uint32_t guid)
{
    assert(node);

    memset(node, 0, sizeof *node);

    node->guid = guid;
    bstv_construct(&node->children);
    ik_quat_set_identity(node->rotation.f);
    ik_vec3_set_zero(node->rotation.f);

    return IK_OK;
}

/* ------------------------------------------------------------------------- */
static void
destroy_recursive(struct ik_node_t* node);
static void
destruct_recursive(struct ik_node_t* node)
{
    NODE_FOR_EACH(node, guid, child)
        destroy_recursive(child);
    NODE_END_EACH

    if (node->effector)
        ik_effector_destroy(node->effector);
    if (node->constraint)
        ik_constraint_destroy(node->constraint);
    if (node->pole)
        ik_pole_destroy(node->pole);

    bstv_clear_free(&node->children);
}
void
ik_node_destruct(struct ik_node_t* node)
{
    assert(node);

    NODE_FOR_EACH(node, guid, child)
        destroy_recursive(child);
    NODE_END_EACH

    if (node->effector)
        ik_effector_destroy(node->effector);
    if (node->constraint)
        ik_constraint_destroy(node->constraint);
    if (node->pole)
        ik_pole_destroy(node->pole);

    ik_node_unlink(node);
    bstv_clear_free(&node->children);
}

/* ------------------------------------------------------------------------- */
static void
destroy_recursive(struct ik_node_t* node)
{
    destruct_recursive(node);
    FREE(node);
}
void
ik_node_destroy(struct ik_node_t* node)
{
    assert(node);

    ik_callbacks_notify_node_destroy(node);
    ik_node_destruct(node);
    FREE(node);
}

/* ------------------------------------------------------------------------- */
ikret_t
ik_node_add_child(struct ik_node_t* node, struct ik_node_t* child)
{
    ikret_t result;
    assert(node);
    assert(child);
    assert(child != node);

    /* May already be part of a tree */
    ik_node_unlink(child);

    /* Searches the entire tree for the child guid -- disabled in release mode
     * for performance reasons */
#ifdef DEBUG
    {
        struct ik_node_t* root = node;
        while (root->parent) root = root->parent;
        if (ik_node_find_child(root, child->guid) != NULL)
        {
            ik_log_warning("Child guid %d already exists in the tree! It will be inserted, but find_child() will only find one of the two.", child->guid);
        }
    }
#endif

    if ((result = bstv_insert(&node->children, child->guid, child)) != IK_OK)
    {
        ik_log_error("Child guid %d already exists in this node's list of children! Node was not inserted into the tree.", child->guid);
        return result;
    }

    child->parent = node;
    return IK_OK;
}

/* ------------------------------------------------------------------------- */
struct ik_node_t*
ik_node_create_child(struct ik_node_t* node, uint32_t guid)
{
    struct ik_node_t* child = ik_node_create(guid);
    if (child == NULL)
        goto create_child_failed;
    if (ik_node_add_child(node, child) != IK_OK)
        goto add_child_failed;

    return child;

    add_child_failed    : ik_node_destroy(child);
    create_child_failed : return NULL;
}

/* ------------------------------------------------------------------------- */
void
ik_node_unlink(struct ik_node_t* node)
{
    assert(node);

    if (node->parent == NULL)
        return;

    bstv_erase(&node->parent->children, node->guid);
    node->parent = NULL;
}

/* ------------------------------------------------------------------------- */
vector_size_t
ik_node_child_count(const struct ik_node_t* node)
{
    return bstv_count(&node->children);
}

/* ------------------------------------------------------------------------- */
struct ik_node_t*
ik_node_find_child(const struct ik_node_t* node, uint32_t guid)
{
    struct ik_node_t* found = bstv_find(&node->children, guid);
    if (found != NULL)
        return found;

    if (node->guid == guid)
        return (struct ik_node_t*)node;

    NODE_FOR_EACH(node, child_guid, child)
        found = ik_node_find_child(child, guid);
        if (found != NULL)
            return found;
    NODE_END_EACH

    return NULL;
}

/* ------------------------------------------------------------------------- */
struct ik_node_t*
ik_node_duplicate(const struct ik_node_t* node, int copy_attachments)
{
    struct ik_node_t* new_node = ik_node_create(node->guid);
    if (new_node == NULL)
        goto malloc_new_node_failed;

    /* Copy over transform data and other properties */
    new_node->rotation = node->rotation;
    new_node->position = node->position;
    new_node->rotation_weight = node->rotation_weight;
    new_node->dist_to_parent = node->dist_to_parent;
    new_node->user_data = NULL;

    if (copy_attachments)
    {
        if (node->effector != NULL)
        {
            struct ik_effector_t* effector = ik_effector_duplicate(node->effector);
            if (effector == NULL)
                goto copy_child_node_failed;
            ik_effector_attach(effector, new_node);
        }
        if (node->constraint != NULL)
        {
            struct ik_constraint_t* constraint = ik_constraint_duplicate(node->constraint);
            if (constraint == NULL)
                goto copy_child_node_failed;
            ik_constraint_attach(constraint, new_node);
        }
        if (node->pole != NULL)
        {
            struct ik_pole_t* pole = ik_pole_duplicate(node->pole);
            if (pole == NULL)
                goto copy_child_node_failed;
            ik_pole_attach(pole, new_node);
        }
    }

    NODE_FOR_EACH(node, child_guid, child)
        struct ik_node_t* new_child_node = ik_node_duplicate(child, copy_attachments);
        if (new_child_node == NULL)
            goto copy_child_node_failed;
        ik_node_add_child(new_node, new_child_node);
    NODE_END_EACH

    return new_node;

    copy_child_node_failed  : ik_node_destroy(new_node);
    malloc_new_node_failed  : return NULL;
}

/* ------------------------------------------------------------------------- */
static void
recursively_dump_dot(FILE* fp, const struct ik_node_t* node)
{
    if (node->effector != NULL)
        fprintf(fp, "    %d [color=\"1.0 0.5 1.0\"];\n", node->guid);

    NODE_FOR_EACH(node, guid, child)
        fprintf(fp, "    %d -- %d;\n", node->guid, guid);
        recursively_dump_dot(fp, child);
    NODE_END_EACH
}

/* ------------------------------------------------------------------------- */
void
ik_node_dump_to_dot(const struct ik_node_t* node, const char* file_name)
{
    FILE* fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        ik_log_error("Failed to open file %s", file_name);
        return;
    }

    fprintf(fp, "graph graphname {\n");
    recursively_dump_dot(fp, node);
    fprintf(fp, "}\n");

    fclose(fp);
}