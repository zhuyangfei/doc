# shell

## Table of contents
- [user scenarios](#user-scenarios)

## user scenarios
### parallel execution
cmd &, for example
```bash
ls &
./script1.sh &
./script2.sh &
wait
```
wait ls, script1 and script2 parallel execution finish
[run cmd parallel in bash](https://www.cyberciti.biz/faq/how-to-run-command-or-code-in-parallel-in-bash-shell-under-linux-or-unix/)
[monitor parallel execution state](https://www.golinuxcloud.com/run-shell-scripts-in-parallel-collect-exit-status-process/#:~:text=To%20run%20script%20in%20parallel%20in%20bash%2C%20you,background%20so%20you%20can%20print%20proper%20exit%20status.)

### loop
```bash
for i in {1..5}
do
    # do somethings
done
for (( i=0; i<10; i++ ))
do
    # do somethings
done
for script in /tmp/scripts/*; do
    # do somethings
done
```

### pass arguments
mkdir.sh folder1
```bash
mkdir $1
```
$0, is the bash file name
[pass arguments in bash](https://www.geeksforgeeks.org/bash-script-how-to-use-command-line-arguments/)
