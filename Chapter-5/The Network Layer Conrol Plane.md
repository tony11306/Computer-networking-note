# The Network Layer: Control Plane

## 5.2 Routing Algorithms

- Centralized routing algorithm
- Decentralized routing algorithm

- static routing algorithms
- dynamic routing algorithms

load-sensitive algorithm

### 5.2.1 The Link-State (LS) routing Algorithm

#### Dijkstra shortest path

Dijkstra 是單源最短路徑演算法，將整個演算法跑過，可以得到起始點到所有點的最短路徑。

    1. 首先只將起始點臨接的節點接起來，其他非臨接的節點距離都是設為無限大。
    2. 接著透過佇列中的邊來不斷更新起點到所有點的最短路徑，使用優先佇列可以優化。
    3. 藉由優先佇列的貪心算法可以得到最短路徑。

若 link cost 跟流量有關，dijkstra 可能會有震盪 (Oscillations) 的問題。所謂震盪就是指當一個路由器是流量最低的狀況時，這時 dijkstra 演算法計算後所有的流量都會灌到這台路由器，使之流量提高，接著再計算後發現流量太高，再把這台路由器的流量抽走，反覆升升降降。

Dijkstra 無法處理負權圖。

#### Bellman-Ford(Distance vector)

Bellman-Ford 也是一個單源最短路徑演算法，相比於 Dijkstra，Bellman-Ford 可以處理負權重的邊。

主要原理是點與點之間會一直交換訊息，若自身節點有任何變動，那麼需要通知相鄰的節點。DV table 會一直更新。

缺點是因為傳送速度慢，訊息可能會有延遲，造成迴路的情況。

## 5.3 Intra-AS Routing in the Internet: OSPF

#### AS(Autonomous system)

**自治系統(AS)**，簡單來說就是第一章所提到的，網路中的網路概念，在 internet 裡面不會每個人都有自己的 router，這樣路由演算法幾乎不可能會收斂，因此會對 router 做分群，可能 A 大學管理一個群，B 大學管理一個群，C 大學管理一個群，每一個大群都稱做一個 AS。假如 A 大學內部某台電腦要和 C 大學內某個電腦溝通，A 大學內部的電腦需要一路路由到最上層路由器，接著透過 A B C 大學的 AS 來做路由，最後送到 C 時再一路向下路由到目的地。

在一個 AS 系統內運作的路由演算法稱作 **intra-AS routing protocol**。

這部影片解釋的很簡單易懂，看不懂的可以再看。

[![IMAGE ALT TEXT HERE](https://i.ytimg.com/vi_webp/ydq4tieEqXE/maxresdefault.webp)](https://www.youtube.com/watch?v=ydq4tieEqXE&ab_channel=internet-class)

#### OSPF(Open shortest path first)

OSPF 是一個使用 flooding 和 dijkstra algorithm 的 link-state 路由演算法，flooding 可以理解為對網路拓樸的暴搜，直到傳到目的地為止。

在 OSPF 中，由於是在一個 AS 裡面，所以 router 知道整個拓樸，可以用 dijkstra 計算到其他節點的最短路徑，並且將計算好的資訊廣播給所有的路由器。

若節點之間有路徑的改變，會馬上廣播給整個系統來更新，就算沒有改變也會每隔一段時間廣播資訊(課本上寫至少 30 分鐘會一次)。

使用 OSPF 有以下好處:
- **Security**: AS 系統內的訊息交換是可以被認證的，藉由簡單的密碼或者是 MD5 雜湊可以做到保密的效果。
- **Multiple same-cost paths**: 由於是 flooding 演算法，所以如果有多條路徑是最短的，那麼這些路徑都能夠被使用，而不會全部資料都放在一條路徑上。
- **Integrated support for unicast and multicast routing**
- **Support for hierarchy within a single AS**: AS 內可以再切割成多個區域，一個區域內可以使用自己的 OSPF link-state 演算法來路由，每個區域內至少一台 border router 負責與外界溝通。在所有區域中會有一個區域叫做 **backbone area** 負責路由整個 AS 系統內的 border routers。

## 5.4 Routing Among the ISPs: BGP

在上一節提到的 OSPF 是屬於 AS 系統內部的路由演算法，外部的路由方是是給 BGP(Border Gateway Protocol) 來計算的。BGP 協議在網路中的重要性媲美 IP 協議。

### 5.4.1 The Role of BGP

BGP 在網路中的功能有:
1. **Obtain prefix reachability information from neighboring ASs**: BGP 允許個子網路去廣播自己在網路中的存在
2. **Determine the best routes to the prefixex**: 透過學習並使用 BGP route-selection procedure 來選擇最佳路徑。

### 5.4.2 Advertising BGP Route Information

BGP 訊息連接方式有分為 iBGP(internal) 和 eBGP(external)。

![ASs with prefix x](imgs/ASs%20with%20prefix%20x.png)

以上圖來說，在 AS1 裡面的路由器都是由 iBGP 連接的，而 1c 跟 2a 和 2c 跟 3a 則是使用 eBGP 連接的。

![iBGP and eBGP](imgs/iBGP%20and%20eBGP.png)

若有一個 prefix 為 x 的子網路在 AS3 的 3d 中，想要告知全部路由器他的存在時，過程如下

        1. 3d 利用 iBGP 來告知 AS3 內的路由器。
        2. 接著到 AS2 的 border gateway 時傳 eBGP 訊息 "AS3 x" 給 AS2 的路由器。AS2 將路由資料傳給整個 AS
        3. 最後到 AS1 時，AS2 傳 eBGP 訊息給 AS1 "AS2 AS3 x"，最後 AS1 將路由資料傳給整個 AS。

### 5.4.3 Determining the Best Routes

在探討演算法之前，需要知道一些術語。當 BGP 傳遞 prefix 訊息時，訊息內部有各種 **BGP attributes**，prefix 和 attributes 稱作一個 **route**，最重要的兩個屬性為 AS-PATH 和 NEXT-HOP。

- AS-PATH: 表示到達一路由器沿途需經過的路由器列表，可以用來防止 looping 的出現。
- NEXT-HOP: 指要往下一個 AS 走的路由器 IP。

#### Hot Potato Routing

熱馬鈴薯路由(或叫燙手山芋?)，是一種自私的演算法，當拿到資料後彷彿燙手山芋，想立即直接往外丟。一個路由器收到資料後，他會找離他最近的 border gateway 路由器傳給他(即便走的路徑更長)，然後往外 forward 給別的 AS。

![hot potato routing](imgs/hot%20potato%20routing.png)


#### Route-Selection Algorithm

[詳細內容](http://www.tsnien.idv.tw/Internet_WebBook/chap6/6-15%20BGP%20%E8%B7%AF%E5%BE%91%E5%8D%94%E5%AE%9A.html)

        1. 藉由 local preference 來先篩選出最高優先度路徑，local preference 的設定交由網管人員設定。
        2. 從這些選出來的路徑中選擇 AS-PATH 最短的出來，使用 DV 演算法求得。
        3. 再從剩餘的路徑中使用熱馬鈴薯路由。
        4. 若還有多餘一條的則利用 BGP 的 identifiers 篩選。

## 5.5 The SDN Control Plane


## 5.6 ICMP: The Internet Control Message Protocol

ICMP 如其名 Internet Message Control Protocol，他就是負責處理網路訊息的協議。

![ICMP message types](imgs/ICMP%20message%20types.png)

在前面章節有提到的 Ping 和 Traceroute 都有用到這個協議，例如 A ping B，A 會送出一個 echo-request 的 ICMP 封包，B 收到封包後會依據設置決定要不要回傳 echo-reply 封包，一般都能夠回傳，除非被防火牆擋掉，或是作業系統直接拒絕回應。

Traceroute 也是用到 ICMP 的訊息傳遞，具體內容[請參考](https://blog.xuite.net/rockmansyz/twblog/115535120)。

