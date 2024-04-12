# docker

## docker image
`docker image`以layer的方式，进行构建。如图：

- `docker container`
`docker container`与`docker image`的不同点在于，`docker container`在`docker image`的只读层上，叠加了可读写的层。
详见：
https://zhuanlan.zhihu.com/p/70424048
https://docs.docker.com/storage/storagedriver/

### image size
不同的Images可以共享相同的layer，因此，Image的大小与实际使用的存储空间并一致。Image之间的layer复用率更高，其占用总空间也越少。
- 构建docker image的最佳实践
  - 根据修改频率，进行build
    修改频率低的先build，即放在下层，如os layer，修改频率高的后build，如APP layer。
  - 分阶段build
  - 使用dockerfile linter工具
  详见：
1. https://medium.com/@sushantkapare1717/how-to-reduce-docker-image-size-best-practices-c5c3ff71da76
2. https://semaphoreci.com/blog/2018/03/14/docker-image-size.html
3. https://hadolint.github.io/hadolint/
