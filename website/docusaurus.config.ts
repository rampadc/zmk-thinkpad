import {themes as prismThemes} from 'prism-react-renderer';
import type {Config} from '@docusaurus/types';
import type * as Preset from '@docusaurus/preset-classic';

const config: Config = {
  title: 'ZMK-ThinkPad',
  tagline: 'Classic ThinkPad keyboards, made wireless and open.',
  favicon: 'img/favicon.svg',
  future: {v4: true},
  url: 'https://congx.dev',
  baseUrl: '/zmk-thinkpad/',
  organizationName: 'rampadc',
  projectName: 'zmk-thinkpad',
  deploymentBranch: 'gh-pages',
  trailingSlash: false,
  onBrokenLinks: 'throw',
  i18n: {defaultLocale: 'en', locales: ['en']},
  presets: [
    [
      'classic',
      {
        docs: {
          path: '../docs',
          routeBasePath: '/',
          sidebarPath: './sidebars.ts',
          editUrl: ({docPath}) =>
            `https://github.com/rampadc/zmk-thinkpad/edit/main/docs/${docPath}`,
          exclude: ['datasheets/**'],
        },
        blog: false,
        theme: {customCss: './src/css/custom.css'},
      } satisfies Preset.Options,
    ],
  ],
  themeConfig: {
    metadata: [{name: 'keywords', content: 'ThinkPad, ZMK, mechanical keyboard, TrackPoint, nRF52840'}],
    colorMode: {defaultMode: 'dark', respectPrefersColorScheme: true},
    navbar: {
      title: 'ZMK-ThinkPad',
      logo: {alt: 'ZMK-ThinkPad TrackPoint mark', src: 'img/trackpoint.svg'},
      items: [
        {type: 'docSidebar', sidebarId: 'docsSidebar', position: 'left', label: 'Docs'},
        {href: 'https://github.com/rampadc/zmk-thinkpad', label: 'GitHub', position: 'right'},
      ],
    },
    prism: {theme: prismThemes.github, darkTheme: prismThemes.dracula, additionalLanguages: ['bash', 'cmake']},
  } satisfies Preset.ThemeConfig,
};

export default config;
