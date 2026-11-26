import copy
import importlib.util
import pathlib
import unittest

spec = importlib.util.spec_from_file_location('attribution', pathlib.Path(__file__).parents[1] / 'check_attribution.py')
attribution = importlib.util.module_from_spec(spec)
spec.loader.exec_module(attribution)


class AttributionContract(unittest.TestCase):
    def borrowed(self):
        return dict(body='Upstream-Reuse: MarshalX/yandex-music-api@' + 'a' * 40,
                    provenance={'version': 1, 'adaptations': [dict(repository=attribution.UPSTREAM,
                        revision='a' * 40, source_paths=['yandex_music/utils/request_base.py'],
                        native_paths=['src/protocol/response.cpp'], license='LGPL-3.0-only')]},
                    files=['third_party/provenance.json', 'src/protocol/response.cpp'],
                    available_files=['THIRD_PARTY_NOTICES.md', 'LICENSES/MarshalX-yandex-music-LGPL-3.0.txt'],
                    commits=['protocol: map response envelopes\n\n' + attribution.TRAILER])

    def test_valid_reuse_and_independent_change(self):
        attribution.check(self.borrowed())
        data = self.borrowed()
        data.update(body='Upstream-Reuse: none', files=['scripts/independent.py'], commits=['independent'])
        attribution.check(data)

    def test_missing_trailer_and_false_none_are_rejected(self):
        for mutation in ('trailer', 'none', 'notice', 'provenance'):
            with self.subTest(mutation=mutation):
                data = copy.deepcopy(self.borrowed())
                if mutation == 'trailer': data['commits'] = ['no credit']
                if mutation == 'none': data['body'] = 'Upstream-Reuse: none'
                if mutation == 'notice': data['available_files'] = []
                if mutation == 'provenance': data['files'].remove('third_party/provenance.json')
                with self.assertRaises(ValueError):
                    attribution.check(data)

    def test_duplicate_declaration_and_nontrailer_credit_are_rejected(self):
        data = self.borrowed()
        data['body'] += '\nUpstream-Reuse: none'
        with self.assertRaises(ValueError): attribution.check(data)
        data = self.borrowed()
        data['commits'] = [attribution.TRAILER + '\n\nUnrelated final paragraph']
        with self.assertRaises(ValueError): attribution.check(data)


if __name__ == '__main__':
    unittest.main()
