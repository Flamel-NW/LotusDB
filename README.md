# LotusDB

[Go语言开源项目LotusDB](https://github.com/flower-corp/lotusdb)的改进C语言版本

- 注意 KV目前只支持ASCII字符串类型 代码中所有的size代表字符串和空字符的总长
- Memtable 和 WAL 并非完全绑定 在转变成immutable后会解绑 所以不能mmap
