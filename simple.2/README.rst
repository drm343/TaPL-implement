Simply Typed System 實作
##########################

以 SECD 的類似設計為底層實作，再搭配類似 boron 的語法，最後加入 type system 以及 boron 的 parse language。

修改自 simple，第 1 版做法只能建立一台 machine，此版在使用函數時必須將 machine 傳進去，因此可以建立一台以上的 machine，此修改是為了之後拿到別的專案使用。

TODO:
  1. 完成 base type tag 的檢查。(現階段不檢查 base type，因此只要 type tag 相同就會檢查通過，即使寫成 add: [ int! int! -> hello_world! ] 在 add 1 2 的狀況下還是能執行，但 add add 1 2 3 會因為 add return hello_world! 跟 int! 不同而判定為 type error)
  2. 檢查 primitive function 的 type 是否在 type env 內。(需要建立一個 type env，同時可以部份解決 TODO 1)
