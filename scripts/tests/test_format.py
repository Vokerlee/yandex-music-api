import importlib.util
import pathlib
import subprocess
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('sdk_format', pathlib.Path(__file__).parents[1] / 'format.py')
formatting = importlib.util.module_from_spec(spec)
spec.loader.exec_module(formatting)


class FormattingContract(unittest.TestCase):
    def test_tracked_selection_and_explicit_paths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            subprocess.run(['git', 'init', '-q', str(root)], check=True)
            (root / 'tracked.cpp').touch()
            (root / 'untracked.cpp').touch()
            subprocess.run(['git', 'add', 'tracked.cpp'], cwd=root, check=True)
            self.assertEqual(formatting.tracked_files(root), ['tracked.cpp'])
            with self.assertRaises(ValueError):
                formatting.tracked_files(root, ['untracked.cpp'])

    def test_custom_hook_is_preserved(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            subprocess.run(['git', 'init', '-q', str(root)], check=True)
            hook = root / '.git/hooks/pre-commit'
            hook.write_text('custom hook\n')
            with self.assertRaises(ValueError):
                formatting.install_hook(root)
            self.assertEqual(hook.read_text(), 'custom hook\n')


if __name__ == '__main__':
    unittest.main()
