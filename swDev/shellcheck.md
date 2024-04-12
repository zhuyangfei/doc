# shell check

## conde shell check
```bash
conde shellcheck
conde shellcheck -f <path>
```

## conde shell format
[conde shellcheck](https://waders-infra.gitlabe2-pages.ext.net.nokia.com/conde/static-analysis.html?highlight=shell#run-shellcheck)

## shell best practice
[shell best practice](https://rcp.gitlabe2-pages.ext.net.nokia.com/rcp-docs/development/shellbestpractice.html)

### use `shellCheck` lint
use `shellCheck` to format and lint your script

### shebang
Bash is the only shell scripting language permitted for executables.
The first line of a script should be a shebang in the following forms.

```bash
#!/bin/bash -e
```
`#!/bin/bash`: shebang, it will be replaced auto when file be install in the target system

### Mandatory use `set -e`
```bash
#!/bin/bash -e
```
`-e`: make script exit as soon as possible, if command executing not return 0.

### file extension
script for executing without `sh` extension, but for not executing, it should be with `.sh` extension.
