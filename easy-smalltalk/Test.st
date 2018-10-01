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
parser run: 'a := b.'.
'result: get-b set-a .' printNl.
