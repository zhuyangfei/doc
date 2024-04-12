# cb010643 VF validation

## design
### adetservice
```plantuml
@startuml
package adetservice
{
    class VFValidation
}
@enduml
```
class VFValidation is in charge of waiting VF ready, then call reset timer.
```plantuml
@startuml
class VFValidation
{
  using WaitVFReady = std::function<bool()>;
}
class PodRestartImp
{
    cancelRestartPod()
}
VFValidation -> PodRestartImp
@enduml
```
using std::function to design `strategy` patten, as the strategy of WaitVFReady maybe change later, such as
alternative1: looping check VF ready flag from syscom, which is current solution.
alternative2: receive sctp_up event from kernel.
other alternatives.
```cpp
/**
* a block call, If vf is ready, then WaitVFReady return true
* if process termination, then return false
*/
using WaitVFReady = std::function<bool()>;
```

```plantuml
@startuml
participant checkVFValidation <<thread>>
checkVFValidation -> VFValidation : check
activate VFValidation
VFValidation -> VFValidation : waitVFReady
VFValidation -> PodRestartImp : cancelRestartPod
@enduml
```
### syscom
```plantuml
@startuml
[syscom] -left- getVFState
[adetservice] <<process>>
adetservice -> getVFState
@enduml
```
syscom provide an interface of `getVFState`, if VF state is ready then return true, else return false.
getVFState use to read VF ready flag from shared memory, which will be set by syscomd.
