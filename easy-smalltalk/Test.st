FileStream fileIn: 'Parser.st'.

parser := Parser new.
parser runAndPrint: '1 + 1'.
parser runAndPrint: '1 + 2 * 3'.
parser runAndPrint: '1 + (2 * 3)'.
parser runAndPrint: '1 + 1 - 2. 3 * 5.'.
parser runAndPrint: 'a := 7.'.
parser runAndPrint: 'b := [:x | x + a].'.
parser runAndPrint: 'c := [:x | x].'.
parser runAndPrint: 'c := [:x | x] value: 3.'.
parser runAndPrint: 'c := [:x | x + a] value: 10.'.
parser runAndPrint: 'd := [:x :y | x - y].'.
parser runAndPrint: 'a := 5.'.
parser runAndPrint: '[:x | x + a] value: 10.'.
parser runAndPrint: 'c.'.
parser runAndPrint: 'b value: 20.'.
parser runAndPrint: 'd value: 20 value: 30.'.
parser runAndPrint: 'd value: a value: 30.'.
