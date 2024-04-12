# kubernetes

## components
- Pod
containers
- Service
The service is used to communicate among Pod
- Ingress
The Pod export himself to external
- Deployment
use to create multi-app or replica
- Stateful
use to create the services which have state.
- Volume
use to save persistence data.
- Configmap
Configuration for APP.

## architect
- Master node
  - 4 process
    API server:
    scheduler
    manager controller
    etcd
- worker node
  - 3 process
    kubelet
    container runtime
    kubeproxy
  - pods

## kubectl
one of a client to connect API service.