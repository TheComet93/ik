import ik
import unittest

class TestTreeObject(unittest.TestCase):
    def test_unlink_when_not_linked(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n2.unlink()
        n2.unlink()
        n1.unlink()
        self.assertEqual(len(n1.children), 0)
        self.assertEqual(len(n2.children), 0)

    def test_default_construct(self):
        n = ik.TreeObject()
        self.assertIsNone(n.algorithm)
        self.assertIsNone(n.constraints)
        self.assertIsNone(n.effector)
        self.assertIsNone(n.pole)

        self.assertEqual(n.position.x, 0.0)
        self.assertEqual(n.position.y, 0.0)
        self.assertEqual(n.position.z, 0.0)

        self.assertEqual(n.rotation.x, 0.0)
        self.assertEqual(n.rotation.y, 0.0)
        self.assertEqual(n.rotation.z, 0.0)
        self.assertEqual(n.rotation.w, 1.0)

    def test_construct_with_position(self):
        n = ik.TreeObject(position=ik.Vec3(1, 2, 3))
        self.assertEqual(n.position.x, 1.0)
        self.assertEqual(n.position.y, 2.0)
        self.assertEqual(n.position.z, 3.0)

    def test_construct_with_rotation(self):
        n = ik.TreeObject(rotation=ik.Quat(1, 2, 3, 4))
        self.assertEqual(n.rotation.x, 1.0)
        self.assertEqual(n.rotation.y, 2.0)
        self.assertEqual(n.rotation.z, 3.0)
        self.assertEqual(n.rotation.w, 4.0)

    def test_construct_with_effector(self):
        n = ik.TreeObject(effector=ik.Effector())

    def test_repr(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n3 = n2.create_child()
        s = repr(n1)

    def test_create_child(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()

    def test_read_children(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n3 = n1.create_child()
        n4 = n2.create_child()
        self.assertEqual(len(n1.children), 2)
        self.assertEqual(len(n2.children), 1)
        self.assertEqual(len(n3.children), 0)
        self.assertEqual(len(n4.children), 0)
        self.assertIn(n2, n1.children)
        self.assertIn(n3, n1.children)
        self.assertIn(n4, n2.children)

    def test_write_children_fails(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n3 = n1.create_child()
        with self.assertRaises(TypeError):
            n1.children[0] = n3
        with self.assertRaises(TypeError):
            n1.children[1] = n3
        with self.assertRaises(TypeError):
            del n1.children[0]

    def test_access_children_out_of_bounds(self):
        n1 = ik.TreeObject()
        with self.assertRaises(IndexError):
            n1.children[0]
        with self.assertRaises(IndexError):
            n1.children[-1]

        n2 = n1.create_child()
        with self.assertRaises(IndexError):
            n1.children[1]
        with self.assertRaises(IndexError):
            n1.children[-1]

    def test_unlink(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n3 = n1.create_child()
        n4 = n2.create_child()
        self.assertEqual(len(n1.children), 2)
        self.assertEqual(len(n2.children), 1)
        self.assertEqual(len(n3.children), 0)
        self.assertEqual(len(n4.children), 0)

        n2.unlink()
        self.assertEqual(len(n1.children), 1)
        self.assertEqual(len(n2.children), 1)
        self.assertEqual(len(n3.children), 0)
        self.assertEqual(len(n4.children), 0)

        n3.unlink()
        self.assertEqual(len(n1.children), 0)
        self.assertEqual(len(n2.children), 1)
        self.assertEqual(len(n3.children), 0)
        self.assertEqual(len(n4.children), 0)

        n4.unlink()
        self.assertEqual(len(n1.children), 0)
        self.assertEqual(len(n2.children), 0)
        self.assertEqual(len(n3.children), 0)
        self.assertEqual(len(n4.children), 0)

    def test_unlink_when_not_linked(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n2.unlink()
        n2.unlink()
        n1.unlink()
        self.assertEqual(len(n1.children), 0)
        self.assertEqual(len(n2.children), 0)

    def test_iterate_children(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n3 = n1.create_child()

        l = [x for x in n1.children]
        self.assertIn(n2, l)
        self.assertIn(n3, l)

    def test_get_parent(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n3 = n2.create_child()

        self.assertIs(n3.parent, n2)
        self.assertIs(n2.parent, n1)
        self.assertIs(n1.parent, None)

    def test_modify_parent_fails(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()

        with self.assertRaises(AttributeError):
            n1.parent = ik.TreeObject()
        with self.assertRaises(AttributeError):
            del n1.parent
        with self.assertRaises(AttributeError):
            n2.parent = ik.TreeObject()
        with self.assertRaises(AttributeError):
            del n2.parent

    def test_set_single_child(self):
        n1 = ik.TreeObject()
        n2 = ik.TreeObject()
        n1.children = n2

        self.assertIs(n2.parent, n1)
        self.assertIn(n2, n1.children)

    def test_set_multiple_children(self):
        n1 = ik.TreeObject()
        n2 = ik.TreeObject()
        n3 = ik.TreeObject()
        n1.children = (n2, n3)

        self.assertIs(n2.parent, n1)
        self.assertIs(n3.parent, n1)
        self.assertIn(n2, n1.children)
        self.assertIn(n3, n1.children)

    def test_set_single_child_again(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n1.children = n2

        self.assertIs(n2.parent, n1)
        self.assertIn(n2, n1.children)

    def test_set_multiple_children_overwrite_existing(self):
        n1 = ik.TreeObject()
        n2 = ik.TreeObject()
        n3 = ik.TreeObject()
        n4 = n1.create_child()
        n1.children = (n2, n3)

        self.assertIs(n2.parent, n1)
        self.assertIs(n3.parent, n1)
        self.assertIn(n2, n1.children)
        self.assertIn(n3, n1.children)

    def test_set_multiple_children_overwrite_existing_same(self):
        n1 = ik.TreeObject()
        n2 = n1.create_child()
        n3 = ik.TreeObject()
        n1.children = (n2, n3)

        self.assertIs(n2.parent, n1)
        self.assertIs(n3.parent, n1)
        self.assertIn(n2, n1.children)
        self.assertIn(n3, n1.children)

    def test_access_parent_after_destroyed(self):
        n = ik.TreeObject().create_child()
        n = n.parent

        self.assertIs(n, None)