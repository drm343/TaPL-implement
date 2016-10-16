Simply Typed System 實作
##########################

以 SECD 的類似設計為底層實作，再搭配類似 boron 的語法，最後加入 type system 以及 boron 的 parse language。

修改自 simple，第 1 版做法只能建立一台 machine，此版在使用函數時必須將 machine 傳進去，因此可以建立一台以上的
 machine，此修改是為了之後拿到別的專案使用。
