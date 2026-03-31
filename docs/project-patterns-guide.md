# MyCameraClient 设计模式与流程说明

## 文档目的

这份文档不是只列出“项目里用了哪些模式”，而是要把下面这些问题说清楚：

1. 这个项目里哪些类分别承担了什么角色。
2. 某个函数为什么要这样写。
3. 一个动作从按钮点击到界面更新，中间经过了哪些类。
4. 单例、工厂、多态、MVD、观察者、生产者-消费者这些概念，在当前项目里分别落在哪些文件上。

当前项目根目录结构里，最核心的代码区域是：

- `src/mainwindow.*`
- `src/ControlWidget/`
- `src/ViewWidget/`
- `src/ParamWidget/`
- `src/CameraInterface/`
- `src/Listener.*`

---

## 一、先用自然语言概括整个项目

这个项目可以先理解成三层：

### 1. 界面层

负责和用户交互的类：

- `MainWindow`
- `ControlWidget`
- `ViewWidget`
- `ParamWidget`

它们负责：

- 按钮点击
- 相机列表显示
- 参数树显示和编辑
- 图像显示

### 2. 业务协调层

负责统一管理相机对象和事件流的类：

- `CameraContext`
- `ListenerManager`

它们负责：

- 相机对象统一入口
- 序列号到相机对象的映射
- 模块间事件广播

### 3. 设备实现层

真正表示“相机对象”的类：

- `CameraInterface`
- `VirtualCamera`
- `HikCamera`

它们负责：

- 是否连接
- 是否抓图
- 参数读写
- 图像获取

---

## 二、MainWindow 的作用：装配器，不做业务

文件：

- `src/mainwindow.h`
- `src/mainwindow.cpp`

### 它做了什么

`MainWindow` 的职责很单纯：

1. 创建三个子模块：
   - `ControlWidget`
   - `ViewWidget`
   - `ParamWidget`
2. 把这三个模块放进主界面的三个容器里。
3. 维持主窗口和状态栏。

### 为什么这样设计

因为主窗口最怕变成“巨无霸类”。

如果主窗口自己直接写：

- 枚举相机
- 连接相机
- 刷新参数
- 开线程取图
- 更新图像

那么这个类会变得越来越难维护。

所以当前项目保持了一个好习惯：

**主窗口只负责装配，不负责具体业务。**

这不是某个经典设计模式的名字，但这是整个项目结构清晰的前提。

---

## 三、单例模式家族

当前项目里真正落地的“单例家族”主要有两个：

1. `CameraContext`
2. `ListenerManager`

它们都属于“系统里全局只保留一个对象”的类型。

---

### 3.1 CameraContext：全局唯一的相机总管

文件：

- `src/CameraInterface/CameraContext.h`
- `src/CameraInterface/CameraContext.cpp`

### 这个类解决什么问题

界面层不应该直接拿着 `VirtualCamera` 或 `HikCamera` 到处调。

否则会出现：

- `ControlWidget` 直接依赖具体相机类
- `ViewWidget` 也直接依赖具体相机类
- `ParamWidget` 也直接依赖具体相机类

这样以后换实现、加设备、切换相机类型都会很痛苦。

所以项目里需要一个统一入口：

**所有界面层都先找 `CameraContext`，由它再去找真正的相机对象。**

### 关键函数

#### `CameraContext::Instance()`

作用：

- 获取全局唯一的 `CameraContext`

为什么这样实现：

- 让任何地方都能通过同一个入口拿到统一的相机管理器
- 避免同一时刻出现多个相机总管，各自维护不同的状态

#### `CameraContext::Release()`

作用：

- 释放单例对象

#### `CameraContext::EnumerationCamera(...)`

作用：

- 枚举当前能使用的相机
- 生成给 UI 显示的 `CameraMetaInfo`
- 同时在内部创建真实相机对象并保存进 `m_serialCamMap`

#### `CameraContext::connect(...) / disconnect(...)`

作用：

- 统一管理连接和断开流程

#### `CameraContext::startGrabbing(...) / stopGrabbing(...)`

作用：

- 统一管理采图状态切换

#### `CameraContext::getParamList(...) / writeParam(...) / getImage(...)`

作用：

- 统一管理参数读取、参数写回、图像获取

### 成员变量为什么重要

#### `QMap<QString, CameraInterface*> m_serialCamMap`

这是 `CameraContext` 最核心的数据。

它的意义是：

- key：相机序列号
- value：相机对象指针

比如：

- `"VIR-0001"` -> `VirtualCamera*`
- `"VIR-0002"` -> `VirtualCamera*`

这样上层只要知道序列号，就能通过 `CameraContext` 找到那台具体相机。

### 在模式里的角色

在单例模式里：

- `CameraContext` 是单例对象本体

在工厂 + 多态模式里：

- `CameraContext` 又是“统一入口”和“创建分发者”

---

### 3.2 ListenerManager：全局唯一的事件广播器

文件：

- `src/Listener.h`
- `src/Listener.cpp`

### 这个类解决什么问题

如果 `ControlWidget` 想通知 `ParamWidget` 和 `ViewWidget` 刷新，
最原始的写法是：

- 一个个 `connect`
- 或者直接硬调用别人的成员函数

这样一开始能跑，但模块越来越多时：

- 连接关系会越来越散
- 主窗口会变成中转站
- 模块之间直接耦合变重

所以项目里引入了 `ListenerManager`：

**统一管理“谁关心什么事件”，以及“谁来广播事件”。**

### 关键函数

#### `ListenerManager::Instance()`

作用：

- 获取全局唯一的监听管理器

#### `registerMessage(int messageMask, Listener* listener)`

作用：

- 注册监听者和它关心的事件集合

#### `unregisterListener(Listener* listener)`

作用：

- 对象销毁时取消注册，避免悬空指针

#### `notify(int message)`

作用：

- 广播某个事件
- 遍历所有注册者
- 找出对这个事件感兴趣的监听者并调用 `RespondMessage(...)`

### 在模式里的角色

在单例模式里：

- `ListenerManager` 是事件系统的全局唯一实例

在观察者模式里：

- 它扮演“事件分发中心”

---

## 四、工厂模式 + 多态

当前项目里，工厂模式和多态不是分开的，而是连在一起工作的。

---

### 4.1 CameraInterface：抽象接口，不是具体相机

文件：

- `src/CameraInterface/CameraInterface.h`

### 它是什么

`CameraInterface` 不是某一台真的相机。

它更像一份“规则说明书”：

- 相机应该能连接
- 相机应该能断开
- 相机应该能开始采图
- 相机应该能停止采图
- 相机应该能读参数
- 相机应该能写参数
- 相机应该能给出图像

### 关键函数

- `isConnect()`
- `isGrabbing()`
- `acquire()`
- `release()`
- `connect()`
- `disconnect()`
- `startGrabbing()`
- `stopGrabbing()`
- `getParamList(...)`
- `writeParam(...)`
- `getImage(...)`

这些函数都是纯虚函数。

为什么写成纯虚函数：

- 表示“所有具体相机都必须实现”
- 上层代码只依赖这套抽象能力，而不依赖某个具体品牌

### 在模式里的角色

在多态模式里：

- `CameraInterface` 是父类接口

---

### 4.2 VirtualCamera：具体相机实现

文件：

- `src/CameraInterface/VirtualCamera.h`
- `src/CameraInterface/VirtualCamera.cpp`

### 它解决什么问题

一开始就接真实海康 SDK 很容易把学习节奏打乱：

- 设备环境不稳定
- 依赖多
- 调试复杂

所以先做 `VirtualCamera`：

**让整套 GUI 和业务流先能完整跑起来。**

### 它保存了哪些状态

最核心的成员：

- `m_isConnected`
- `m_isGrabbing`
- `m_frameIndex`
- `m_cameraParams`

### 关键函数

#### `connect()`

作用：

- 让虚拟相机进入“已连接”状态

#### `disconnect()`

作用：

- 让虚拟相机进入“未连接”状态
- 同时结束抓图状态

#### `startGrabbing()` / `stopGrabbing()`

作用：

- 切换采图状态

#### `getParamList(...)`

作用：

- 把内部参数数组返回出去

#### `writeParam(...)`

作用：

- 根据参数名查找并更新 `m_cameraParams`

#### `getImage(...)`

作用：

- 生成一张假的 `QImage`
- 模拟“相机持续吐图”

### 在模式里的角色

在工厂模式里：

- `VirtualCamera` 是被工厂创建出来的具体产品

在多态里：

- 它是 `CameraInterface` 的一个子类实现

---

### 4.3 HikCamera：真实设备接入骨架

文件：

- `src/CameraInterface/HikCamera.h`
- `src/CameraInterface/HikCamera.cpp`

### 它当前是什么状态

它现在还是接入骨架：

- 已经实现了和 `CameraInterface` 一致的函数签名
- 但没有接真实海康 SDK
- 当前默认返回 `SDK_NOT_AVAILABLE`

### 为什么先这样做

因为当前目标是：

1. 先把接口和架构立住
2. 再接真实 SDK

这样上层结构不会因为底层 SDK 接入而被推翻。

### 在模式里的角色

在工厂模式里：

- `HikCamera` 是另一种“具体产品”

在多态里：

- 它和 `VirtualCamera` 一样，都是 `CameraInterface` 的实现

---

### 4.4 CameraContext 的工厂角色

文件：

- `src/CameraInterface/CameraContext.cpp`

### 当前项目里工厂落在哪

最关键函数：

- `createCamera(const CameraMetaInfo& info)`

它根据：

- `info.Backend`

来决定创建：

- `VirtualCamera`
- `HikCamera`

这就是当前项目里的“工厂入口”。

### 为什么这样实现

如果 `EnumerationCamera()` 里直接到处写：

- `new VirtualCamera(...)`
- 以后再手插 `new HikCamera(...)`

创建逻辑会越来越散。

把创建动作集中到 `createCamera(...)` 的好处是：

- 创建策略集中
- 上层不用关心具体类名
- 后续扩展更多相机实现更方便

### 多态怎么在这里发生

`m_serialCamMap` 保存的是：

- `CameraInterface*`

但实际指向的可能是：

- `VirtualCamera`
- `HikCamera`

当 `CameraContext` 写：

```cpp
camera->connect();
camera->getImage(image);
camera->writeParam(param);
```

虽然变量类型是父类指针，真正执行的却是当前对象自己的实现。

这就是多态。

---

## 五、MVD 模式：参数区的核心

当前项目里最标准地使用了 MVD 的地方，就是右侧参数面板。

MVD 这里可以理解成：

- `Model`：数据模型
- `View`：显示视图
- `Delegate`：单元格编辑器代理

---

### 5.1 View：QTreeView 只负责显示，不负责存数据

文件：

- `src/ParamWidget/paramwidget.ui`
- `src/ParamWidget/paramwidget.cpp`

### 它做了什么

`ParamWidget` 构造函数里：

- `ui->paramTreeView->setModel(m_paramModel);`
- `ui->paramTreeView->setItemDelegate(m_paramDelegate);`

意思是：

- 树里显示什么，由 `Model` 决定
- 双击编辑时用什么控件，由 `Delegate` 决定

### 为什么这样设计

如果直接拿 `QTreeWidget` 手工塞数据，短期简单，但后面会越来越难维护：

- 分组树结构难扩展
- 编辑逻辑散
- 参数读写和显示混在一起

所以现在让 `QTreeView` 只做一件事：

**显示模型里的树。**

---

### 5.2 CameraParamItem：树节点对象

文件：

- `src/ParamWidget/CameraParamItem.h`
- `src/ParamWidget/CameraParamItem.cpp`

### 它是什么

它表示参数树里的一个节点。

这个节点可能是：

1. 组节点
   - 比如 `AcquisitionControl`
2. 参数节点
   - 比如 `ExposureTime = 12000 us`

### 它保存什么

- 子节点数组
- 父节点指针
- 每列显示值
- 完整的 `CameraParam`
- 当前节点是否真的是参数节点

### 为什么需要它

因为树模型不能只存字符串。

项目还需要：

- 父子关系
- 分组结构
- 每个参数的完整信息

所以需要一个专门的节点类来承载这些内容。

---

### 5.3 CameraParamModel：参数树的数据中枢

文件：

- `src/ParamWidget/CameraParamModel.h`
- `src/ParamWidget/CameraParamModel.cpp`

### 它解决什么问题

`QTreeView` 不知道参数树怎么组织。

它只会不断问：

- 有几行？
- 有几列？
- 这个格子显示什么？
- 这个格子能不能编辑？
- 改完值以后怎么更新？

这些都由 `CameraParamModel` 回答。

### 关键函数

#### `index(...)`

作用：

- 把行列坐标变成 `QModelIndex`

#### `parent(...)`

作用：

- 告诉 Qt 树节点的父子关系

#### `rowCount(...)`

作用：

- 告诉 Qt 每个节点下面有多少个孩子

#### `columnCount(...)`

作用：

- 告诉 Qt 参数树有几列

#### `data(...)`

作用：

- 返回显示给界面的值
- 也返回隐藏保存的完整 `CameraParam`

这里有个很关键的设计：

- `Qt::DisplayRole` / `Qt::EditRole` 返回显示值
- `Qt::UserRole` 返回完整 `CameraParam`

为什么这样写：

因为后面：

- 点参数看说明
- Delegate 决定编辑器类型
- 修改后写回相机

都需要完整参数对象，而不是只拿一段字符串。

#### `flags(...)`

作用：

- 指定哪些格子可编辑

当前逻辑是：

- 分组节点不能编辑
- 参数名不能编辑
- 只有参数值那一列可以编辑

#### `setData(...)`

作用：

- 接收 Delegate 交回来的新值
- 更新树节点里的显示值
- 更新保存的完整 `CameraParam`
- 发出：
  - `dataChanged(...)`
  - `SigValueChanged(...)`

为什么要额外发 `SigValueChanged(...)`

因为：

- `dataChanged(...)` 只是告诉界面“显示值变了”
- `SigValueChanged(...)` 是业务信号，告诉外层“这个参数真的被改了，可以写回相机了”

#### `addCameraParam(...)`

作用：

- 把平铺的参数数组组织成“组 -> 参数”的树结构

为什么这样写：

因为参数面板不是平铺列表，而是按功能分组更符合工业相机软件习惯。

#### `clear()`

作用：

- 清空并重建模型

---

### 5.4 CameraParamDelegate：编辑器工厂

文件：

- `src/ParamWidget/cameraparamdelegate.h`
- `src/ParamWidget/cameraparamdelegate.cpp`

### 它解决什么问题

不同参数不一定用同一种编辑器。

例如：

- `ExposureTime` 更适合文本输入
- `TriggerMode` 更适合下拉框

所以需要一个单独的对象来决定：

**编辑这个格子时，该弹什么控件。**

### 关键函数

#### `createEditor(...)`

作用：

- 创建真正的编辑控件

当前逻辑：

- 如果参数有 `Options`
  - 创建 `QComboBox`
- 否则
  - 创建 `QLineEdit`

#### `setEditorData(...)`

作用：

- 打开编辑器时，把旧值放进去

#### `setModelData(...)`

作用：

- 编辑结束后，把新值交回 `Model`

注意：

`Delegate` 不直接改 `VirtualCamera`，也不直接改 `CameraContext`。

它只做一件事：

**把编辑器里的新值交回 Model。**

#### `updateEditorGeometry(...)`

作用：

- 让编辑器出现在正确的单元格位置

### 在模式里的角色

在 MVD 里：

- 它就是 `Delegate`

---

### 5.5 ParamWidget：把 MVD 和业务层粘起来

文件：

- `src/ParamWidget/paramwidget.h`
- `src/ParamWidget/paramwidget.cpp`

### 它做了什么

`ParamWidget` 自己并不是 Model，也不是 Delegate。

它更像“参数区控制器”，负责：

1. 从 `CameraContext` 读取参数列表
2. 交给 `CameraParamModel`
3. 响应选中项变化，显示参数说明
4. 响应 `SigValueChanged(...)`，把参数写回相机

### 关键函数

#### `refreshInfoPanel()`

作用：

- 刷新右侧整块参数区
- 包括：
  - 当前相机基础信息
  - 连接状态
  - 抓图状态
  - 参数树

#### `refreshParamTree(...)`

作用：

- 清空旧模型
- 把新参数全部交给 `Model`
- 展开树

#### `onSelectionChanged(...)`

作用：

- 当参数树当前选中项变化时
- 从 `UserRole` 取出完整 `CameraParam`
- 显示参数说明

#### `writeCameraParam(...)`

作用：

- 当模型发出 `SigValueChanged(...)` 时
- 取出被修改的 `CameraParam`
- 通过 `CameraContext::writeParam(...)` 写回相机

### 为什么 ParamWidget 不能直接改 Model 就算结束

因为：

- Model 里的数据只是界面这一侧的树数据
- 真正的业务状态还在 `VirtualCamera`

所以必须有最后一步：

`ParamWidget -> CameraContext -> VirtualCamera`

这样界面和业务对象才保持一致。

---

## 六、观察者模式：事件总线系统

当前项目里观察者模式的落地点是：

- `Listener`
- `ListenerManager`
- `ControlWidget`
- `ViewWidget`
- `ParamWidget`

---

### 6.1 Listener：观察者接口

文件：

- `src/Listener.h`

### 它做了什么

定义了一个统一接口：

- `RespondMessage(int message)`

谁想接收事件，谁就实现它。

### 为什么这样设计

这样 `ListenerManager` 就不需要知道具体类名。

它只知道：

- 这是一个 `Listener`
- 我可以把消息交给它处理

---

### 6.2 ListenerManager：观察者模式里的“通知中心”

文件：

- `src/Listener.cpp`

### 它怎么工作

#### `registerMessage(...)`

表示：

- 某个监听者关心哪些消息

#### `notify(...)`

表示：

- 某个事件发生了
- 广播给所有已注册监听者

### 为什么要这样做

如果没有它，模块之间就只能：

- 直接互相调用
- 或者主窗口到处 `connect(...)`

模块越多，耦合越乱。

有了它以后：

- 事件发起者只负责广播“发生了什么”
- 响应者自己决定收到消息后做什么

这就是观察者模式最核心的价值。

---

### 6.3 ControlWidget：主要事件发起者

文件：

- `src/ControlWidget/controlwidget.cpp`

### 它在做什么

#### `onEnumerateClicked()`

作用：

- 触发枚举相机
- 枚举成功后广播 `CAMERA_ENUMERATION`

#### `cameraListWidget::currentRowChanged`

作用：

- 当前选中相机变化
- 广播 `CAMERA_SWITCH`

#### `onConnectClicked()`

作用：

- 连接成功后广播 `CAMERA_CONNECT`
- 断开成功后广播 `CAMERA_DISCONNECT`

### 为什么这样设计

`ControlWidget` 不再直接告诉 `ParamWidget` 或 `ViewWidget`：

- “你去刷新”

而是只广播：

- “发生了枚举”
- “发生了切换”
- “发生了连接”

这比直接硬调用别的模块更松耦合。

---

### 6.4 ViewWidget 和 ParamWidget：主要事件接收者

文件：

- `src/ViewWidget/viewwidget.cpp`
- `src/ParamWidget/paramwidget.cpp`

### ViewWidget

它注册关心：

- 枚举
- 相机切换
- 连接
- 断开
- 开始抓图
- 停止抓图

收到消息后：

- 刷新按钮状态
- 必要时清空预览区

### ParamWidget

它同样注册关心这些消息。

收到消息后：

- 刷新相机信息
- 刷新参数树
- 刷新参数区的禁用/启用状态

### 为什么这属于观察者模式

因为现在的关系已经变成：

- `ControlWidget` / `ViewWidget` 只是发布事件
- `ParamWidget` / `ViewWidget` 自己监听并响应

而不是模块之间彼此硬编码依赖。

---

## 七、生产者 - 消费者队列

这里要特别讲清楚：

当前项目没有自己写一个显式的“图像队列类”，比如 `ImageQueue`。

但是从运行机制上，它已经形成了一个**轻量版的生产者 - 消费者模型**。

---

### 7.1 谁是生产者

文件：

- `src/ViewWidget/AcquireImageProcess.h`
- `src/ViewWidget/AcquireImageProcess.cpp`

`AcquireImageProcess` 是生产者。

### 它做了什么

#### `run()`

作用：

- 在线程里循环拉图

流程是：

1. 读取当前串口号快照
2. 调 `CameraContext::getImage(...)`
3. 拿到 `QImage`
4. 发出 `imageReady(serial, image)`

### 为什么要放在线程里

因为取图是重复执行的工作。

如果你把它放在 UI 线程里死循环：

- 界面会卡住
- 按钮点不动
- 窗口不能刷新

所以把它放到 `QThread` 子线程里，就是典型的“生产者独立工作”。

---

### 7.2 谁是消费者

文件：

- `src/ViewWidget/viewwidget.cpp`

`ViewWidget::onImageReady(...)` 是消费者。

### 它做了什么

收到线程发来的：

- `serial`
- `image`

之后：

1. 先判断是不是当前预览的相机
2. 如果不是，丢弃
3. 如果是，把 `QImage` 转成 `QPixmap`
4. 显示到 `imageLabel`

### 为什么这样做

因为：

- 生产者线程只负责取图
- 消费者界面线程只负责显示图

这就避免了在子线程里直接操作 UI 控件。

---

### 7.3 队列到底在哪里

严格说，当前项目没有手写一个 `QQueue<QImage>`。

但 Qt 的跨线程信号槽本身就提供了一个**隐式消息队列**：

- 子线程 `emit imageReady(...)`
- Qt 把这条消息排到目标线程事件队列里
- UI 线程再执行 `onImageReady(...)`

所以从概念上看，它仍然属于“生产者 - 消费者”。

只是现在的队列不是你自己手写容器，而是 Qt 事件系统帮你做了。

### 当前这种实现的优点

- 写法简单
- 足够学习和小型项目使用
- 易于理解

### 当前这种实现的限制

- 没有显式缓冲区长度控制
- 没有丢帧策略
- 没有背压处理

所以如果以后做高帧率、高吞吐项目，确实可以再进化成一个显式图像队列类。

---

## 八、这些模式在整个项目里是如何串起来的

你可以把整个项目理解成下面这条大链：

1. `MainWindow`
   - 负责装配界面模块

2. `ControlWidget`
   - 用户操作入口
   - 负责枚举、连接、切换
   - 通过 `ListenerManager` 广播事件

3. `CameraContext`
   - 单例总管
   - 负责创建和管理相机对象
   - 通过多态调用具体相机实现

4. `VirtualCamera / HikCamera`
   - 真正保存相机状态
   - 提供参数和图像

5. `ParamWidget`
   - 用 MVD 管理参数显示和编辑
   - 参数改动后再写回业务层

6. `AcquireImageProcess`
   - 在子线程里生产图像

7. `ViewWidget`
   - 在主线程里消费图像并显示

8. `ListenerManager`
   - 在模块之间广播状态变化

所以这几个模式不是孤立的，而是协作关系：

- **单例** 提供全局唯一入口和事件中心
- **工厂 + 多态** 解决“不同相机实现如何被统一管理”
- **MVD** 解决“参数树如何显示和编辑”
- **观察者** 解决“模块之间如何低耦合联动”
- **生产者 - 消费者** 解决“图像采集如何不阻塞界面”

---

## 九、最值得你牢牢记住的 5 句话

1. `MainWindow` 只负责装配，不负责业务。
2. `CameraContext` 是全局唯一的相机总管。
3. `CameraInterface` 是规则，`VirtualCamera / HikCamera` 是实现。
4. `ParamWidget` 不是自己管参数数据，它通过 `Model + Delegate` 来显示和编辑参数，再通过 `CameraContext` 写回相机。
5. 图像采集在线程里生产，界面在线程外消费，这就是为什么 UI 不会卡死。

---

## 十、你后面复习时的推荐阅读顺序

建议按下面顺序读代码：

1. `src/mainwindow.cpp`
2. `src/ControlWidget/controlwidget.cpp`
3. `src/CameraInterface/CameraContext.cpp`
4. `src/CameraInterface/CameraInterface.h`
5. `src/CameraInterface/VirtualCamera.cpp`
6. `src/Listener.cpp`
7. `src/ParamWidget/CameraParamModel.cpp`
8. `src/ParamWidget/cameraparamdelegate.cpp`
9. `src/ParamWidget/paramwidget.cpp`
10. `src/ViewWidget/AcquireImageProcess.cpp`
11. `src/ViewWidget/viewwidget.cpp`

按这个顺序，你会更容易把“模式”和“调用流程”对应起来。
