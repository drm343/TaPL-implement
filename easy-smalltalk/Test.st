FileStream fileIn: 'Parser.st'.

parser := Parser new.
parser run: '1 + 1'.
'result: 1 1 +' printNl.
parser run: '1 + 2 * 3'.
'result: 1 2 + 3 *' printNl.
parser run: '1 + (2 * 3)'.
'result: 1 2 3 * +' printNl.
parser run: '1 + 1 - 2. 3 * 5.'.
'result: 1 1 + 2 - . 3 5 * .' printNl.
parser run: 'a := 1.'.
'result: 1 set-a .' printNl.
parser run: 'b := 1 + 2.'.
'result: 1 2 + set-b .' printNl.
parser run: 'a := b + 2.'.
'result: get-b 2 + set-a .' printNl.
parser run: 'a := 2 + b.'.
'result: 2 b + set-a .' printNl.
parser run: 'c := [:x | x].'.
'result: [ :x | x ] set-c .' printNl.
parser run: 'c := [:x | x + 1].'.
'result: [ :x | x 1 + ] set-c .' printNl.
parser run: 'c := [:x | x + a].'.
'result: [ :x | x a + ] set-c .' printNl.
parser run: 'c := [:x | a + x].'.
'result: [ :x | a x + ] set-c .' printNl.
parser run: 'c := [:x :y | y + x].'.
'result: [ :x | y x + ] set-c .' printNl.
