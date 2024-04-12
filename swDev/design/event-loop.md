# event loop
event loop is one kind of design patten that wait for and dispatch event or message.

## user scenarios
1. event driven, such as app need to handle many event
2. I/O operating, such as files, socket
3. high performance require, such as non-block, less cpu consuming.

## how to design
1. define event, such as file fd, socket fd
2. design event callback handle
3. monitor events through those fds, for example `poll`, `select` or `epoll`

## Handling signal
`signal` is an asynchronous event.

### data race
If app is blocking on a `select`, then `select` will return early with `EINTR`. At the same time, the signal is executing, so there is data race between event handling and signal handling.
`signalfd()` used to solve this race issue.
https://en.wikipedia.org/wiki/Event_loop
