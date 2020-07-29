【★本文字版README文件不含主界面截图，请前往README.pdf】

SmartChordGen 智弦	2.0 读我文件

程序作者：清华大学  沈智云[1]（程序设计），  星海音乐学院  陈文戈[1]（构想与调试）

（此版本程序为开源，遵循 GNU GPLv3 许可协议；功能更完善的商业应用版本请见文末。）

智弦（SmartChordGen）是清华大学沈智云和星海音乐学院陈文戈共同开发的一款功能强大的和弦进行自动生成软件。该软件是对当代和声理论的“集大成” ，支持十二平均律下10项筛选指标、40余项详细参数，支持MIDI文件输出，并提供一系列和弦库、排列库和预设。目前的最新版本是2.0，在 https://github.com/Chen-and-Sim/SmartChordGen 提供开源下载。

该软件为绿色软件（无需安装即可使用），大小43MB。智弦（SmarChordGen）2.0界面支持中文、英文两种语言。在顶栏选择 “English” 可切换至英文界面， “中文” 返回。详细用法说明请参阅用户手册。
注意：如遇界面显示不正确如文字和按钮错位等，请将Windows显示设置中的“缩放比例”调为100%即可解决。



【功能更完善的商业应用版本：智弦作曲伴侣】

智弦2.0是免费开源的，是项目的雏形。之后我们将推出两个功能较完善的版本应用：


智弦作曲伴侣 Composer Companion         |  售价：RMB 39

包含智弦2.0的全部功能，还有如下亮眼的新功能：

1.“反推器”：输入一个MIDI文件，自动按“时间窗口”设置划分成一个个和弦，并度量它们的参数生成一个.profile文件，包含 .preset、和弦db和排列db的数据（或分开独自输出）。

2.“织体化”：把输出的结果配上用户指定的节奏和织体；“复调化”：添加和弦外音，使和声变成复调。

3.“配和声”：读入一个单声部的MIDI文件，按参数配和声输出，包括声部进行规则（平五平八等） 。

4. “表达式”：支持用以生成确定和弦进行的“蒂氏表达式”（由斯坦福大学Tymoczko教授提出）、“张氏表达式”（由知乎 @本尼桑 提出），以及十二音序列/魔方变换等。

5.辅助工具（ultilities）升级图形界面（GUI）形式，操作更简便；更详细的统计数据显示。



智弦作曲伴侣+ Composer Companion Plus    |  售价：RMB 99

Composer Companion Plus是在Composer Companion的基础上支持微分音（小于半音的任意频率音），同时支持导入Scala音阶（.scl）文件和带弯音的MIDI文件。



（附：智弦 2.0 软件文件夹说明）

该软件为绿色软件（无需安装即可使用），大小43MB。软件文件夹中的各文件说明如下：

\bin\SmartChordGen.exe   主程序
\bin\guide               用户手册
\samples                 示例输出与示例音乐
\source                  源文件（开发语言为C++ /Qt 6.0，目前支持Windows平台）
\presets                 预设文件夹（其中standard-presets为自带示例预设）
\output                  输出文件夹
\db\chord                和弦类型库（用户可用writedb.exe生成新的）
\db\align                和弦排列库（用户可用writealign.exe生成新的）
\utilities\*.exe         辅助工具程序
- Charming Chords
- ChordCheck（检查一个和弦是否属于指定的类型库）
- ChordSect（和弦音集的交、并处理）
- ChordStats（和弦进行的指标统计）
- ChordTrans（和弦移位与倒影）
- dbnormal（把指定和弦库转换为标准型Normal Form）
- dbprime（把指定和弦库转换为最简型Prime Form）
- outputconv（把输出的分析报告文件转化为MIDI音乐格式）
- writedb（根据输入的参数生成新的和弦类型库）
- writealign（根据输入的参数生成新的和弦排列库）


（附：智弦2.0版本更新履历）
2020-06-22：智弦SmartChordGen 1.0 [1.0.0622]（命令行界面的“最原始版本”）。
2020-07-02：智弦SmartChordGen 1.5 [1.5.0702]（加入主程序的辅助工具 —— ultilities文件夹）。
（※注：1系列版本为只有命令行界面的版本，2.0版本已包含其全部功能，因此这些版本已淘汰，不作发行。）
2020-07-19：智弦SmartChordGen 2.0 alpha [2.0.0719]（具备GUI图形界面）。
2020-07-21：智弦SmartChordGen 2.0 beta [2.0.0721]（完善功能并修正了一些bug）。
2020-07-30：智弦SmartChordGen 2.0 正式版 [2.0.0730]（GitHub开源正式发布的版本）。
