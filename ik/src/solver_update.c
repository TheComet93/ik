#include "ik/solver.h"
#include "ik/solver_update.h"
#include "ik/effector.h"
#include "ik/node_data.h"
#include "ik/ntf.h"
#include "ik/vec3.h"
#include "ik/vector.h"

/* ------------------------------------------------------------------------- */
static void
update(struct ik_node_data_t* tip, struct ik_node_data_t* base)
{
    struct ik_effector_t* effector =
        (struct ik_effector_t*)tip->attachment[IK_ATTACHMENT_EFFECTOR];

    /* lerp using effector weight to get actual target position */
    effector->actual_target = effector->target_position;
    ik_vec3_sub_vec3(effector->actual_target.f, tip->transform.t.position.f);
    ik_vec3_mul_scalar(effector->actual_target.f, effector->weight);
    ik_vec3_add_vec3(effector->actual_target.f, tip->transform.t.position.f);

    /* Fancy solver using nlerp, makes transitions look more natural */
    if (effector->features & IK_EFFECTOR_WEIGHT_NLERP && effector->weight < 1.0)
    {
        ikreal_t distance_to_target;
        union ik_vec3 base_to_effector;
        union ik_vec3 base_to_target;

        /* Need distance from base node to target and base to effector node */
        base_to_effector = tip->transform.t.position;
        base_to_target = effector->target_position;
        ik_vec3_sub_vec3(base_to_effector.f, base->transform.t.position.f);
        ik_vec3_sub_vec3(base_to_target.f, base->transform.t.position.f);

        /* The effective distance is a lerp between these two distances */
        distance_to_target = ik_vec3_length(base_to_target.f) * effector->weight;
        distance_to_target += ik_vec3_length(base_to_effector.f) * (1.0 - effector->weight);

        /* nlerp the target position by pinning it to the base node */
        ik_vec3_sub_vec3(effector->actual_target.f, base->transform.t.position.f);
        ik_vec3_normalize(effector->actual_target.f);
        ik_vec3_mul_scalar(effector->actual_target.f, distance_to_target);
        ik_vec3_add_vec3(effector->actual_target.f, base->transform.t.position.f);
    }
}
void
ik_solver_update_effector_targets(struct ik_solver_t* solver)
{
    uint32_t i;
    for (i = 0; i != solver->ntf->node_count; ++i)
    {
        if (NTF_POST_CHILD_COUNT(solver->ntf, i) == 0)
        {
            update(NTF_POST_NODE(solver->ntf, i), NTF_POST_BASE(solver->ntf, i));
        }
    }
}

/* ------------------------------------------------------------------------- */
void
ik_solver_update_node_distances(struct ik_solver_t* solver)
{
}