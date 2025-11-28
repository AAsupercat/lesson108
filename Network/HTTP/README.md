# HTTP协议

## 一些常识知识点

### 域名访问流程
例如输入：`www.baidu.com`
1. 首先是域名转化为ip:port形式，先查看本地缓存，没有找到通过DNS递归搜索，一层一层往上找，找到就访问ip:port；
   - **DNS查询本身使用UDP协议**：DNS查询通常使用UDP 53端口，因为查询请求小、响应快，UDP无连接特性适合这种场景；
2. 传输层协议选择：
   - **TCP协议**：需要建立连接，三次握手建立连接，传输完成后四次挥手断开连接，支持长连接（keep-alive）；
   - **UDP协议**：无连接协议，直接发送数据包，不需要握手过程，适合实时性要求高的场景（如DNS查询、视频流、游戏等）；
3. HTTPS，会建立SSL连接，证书认证，密钥交换，会话密钥，加密交换（基于TCP协议）;如果是UDP，会有QUIC协议，内置加密，解决UDP不可靠问题（HTTP/3）
4. 客户端发送HTTP请求
5. 服务端处理请求并HTTP响应
6. 浏览器接受响应并解析：HTML/CSS等，这个过程中会处理其他资源，重复上述过程
7. 执行一些代码脚本，然后呈现内容

---

### URL的认识
URL（Uniform Resource Locator）统一资源定位符，用于定位网络资源。
当请求关键词有一些URL的特殊关键字时，会进行替换（十六进制HEX）,urlencode/urldecode
**URL的组成部分**：
```
协议://域名:端口/路径?查询参数#锚点
```

**示例**：`https://www.example.com:443/path/to/page?id=123&name=test#section1`

1. **协议（Scheme）**：`http`、`https`、`ftp`等
2. **域名（Host）**：如`www.example.com`
3. **端口（Port）**：默认http:80，https:443（可省略）
4. **路径（Path）**：资源在服务器上的路径，如`/path/to/page`
5. **查询参数（Query）**：`?key=value&key2=value2`，用于传递参数
6. **锚点（Fragment）**：`#section1`，用于定位页面内位置（不发送到服务器）

---

### 端口号的绑定查看
1. 一个进程可以绑定多个端口号；
2. 一个端口号不可以被多个进程绑定；

**端口号绑定：[0~1023]** HTTP: 80,HTTPS: 443,MySQL: 3306,SSH: 22...

### 自定义协议
自定义协议，这里可以模拟实现一下http的请求和响应的数据格式，将其封装成两个类：Request_HTTP、Response_HTTP;

### 序列化与反序列化
序列化与反序列化，一般数据传输不是以结构体传输的，而是将数据结构通过JSON/Protobuf成熟的工具进行处理，处理成字符串。

处理流程：
```
内存中的数据结构 → 序列化 → 字符串/二进制 → 网络传输 → 反序列化 → 内存中的数据结构
```
## HTTP的结构
请求行/响应行、请求报头/响应报头、空行、请求正文/响应正文

空行的意义：用于分离报头和有效载荷，应用层就是分离提取出数据;
### Request_HTTP

#### 请求行：
1. Method 方法：get,post,connect
    - GET方法：用于申请资源，通过url访问资源;
    - POST方法：用于申请资源，通过正文访问资源;
2. URL : 可省略
3. HTTP_Version：http版本

#### 请求报头：
1. Host:目标主机
2. Accept:请求的数据类型：HTML/JSON等
3. Contont-Type:如果有正文，我发送的类型是这个
4. Contont-Length:正文长度
5. Connection:长连接(keep-alive/close)
6. User-Agent:用于记录客户端的信息，比如什么操作系统等，往往是反扒手段
7. Cookie:缓存输入的信息

#### 请求正文

### Response_HTTP

#### 响应行
1. HTTP_Version:
    - http/1.0:默认不支持长连接，短连接;
    - http/1.1:默认支持长连接;
    - http/2.0:
2. 状态码：
    - 1xx:信息性状态码，正在处理请求;
    - 2xx:成功状态码;
    - 3xx:重定向状态码，短期重定向和长期重定向;
    - 4xx:客户端错误状态码;
    - 5xx:服务器错误状态码;
3. 状态描述：

#### 响应报头：
1. Content-Type:我返回的类型是这个
2. Content-length:正文长度
3. Connection:长连接(keep-alive/close)
4. Location:重定向地址
5. Set-Cookie:设置Cookie的信息，方便下次直接使用
#### 响应正文：
