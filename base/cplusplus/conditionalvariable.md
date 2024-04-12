# conditional variable

## spurious wake up
处于等待线程，提前wake up，即wait返回了，但是，并不是由想要的`条件`触发的。
因为在多个线程触发信号的情况下，某个线程，在wait返回之后，重新拿到锁之前，`条件`被其他线程修改了，所以，出现wait返回后，不是你想要的信号。
代码上，wait返回后，要检查特定的业务条件，判断是否重新等待或者继续执行。
https://en.wikipedia.org/wiki/Spurious_wakeup

### note
cv.wait 不会因为系统`中断`而唤醒，所以，进程退出前，要触发信号，通知wait的线程，以做到gracefully exit.
