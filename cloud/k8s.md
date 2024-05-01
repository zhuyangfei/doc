# pod states
https://kubernetes.io/docs/concepts/workloads/pods/pod-lifecycle/
# container states
https://kubernetes.io/docs/concepts/containers/container-lifecycle-hooks/
[how to usage a hook](https://www.learncloudnative.com/blog/2021-05-03-container-lifecycle-hooks)

## network
https://atbug.com/deep-dive-k8s-network-mode-and-communication/

## storage
### storage class
`Storage class`用来指定不同存储类型，如：Ceph,NFS等，配置不同类型存储的行为，如：fast，slow等，即用来配置不同的存储策略。
`PV`通过`Storage class`来创建不同的存储类型及存储策略。`Storage class`之于`PV`，可以类似于设计模式中的策略模式。不同的厂商，提供不同的`SC`给K8s使用。
