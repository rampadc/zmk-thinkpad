# ThinkPad ZMK website

The Docusaurus site publishes the repository's `docs/` directory at the site
root.

## Local preview

```sh
cd website
npm install
npm start
```

## Production build

```sh
cd website
npm run build
```

Pushing a site or documentation change to `main` runs the GitHub Pages workflow. In the repository settings, set **Pages → Build and deployment → Source** to **GitHub Actions** before the first deployment.
