FileStream fileIn: 'Parser.st'.

parser := Parser new.
(parser run: '1 + 1') printNl.
'result: 1 1 +' printNl.
(parser run: '1 + 2 * 3') printNl.
'result: 1 2 3 * +' printNl.
(parser run: '(1 + 2) * 3') printNl.
'result: 1 2 + 3 *' printNl.
"(parser run: '1 + 1 - 2') printNl.
'result: 1 1 + 2 -' printNl.
(parser run: '1 + 2 * 3') printNl.
'result: 1 2 3 * +' printNl.
(parser run: '1 * 2 + 3') printNl.
'result: 1 2 * 3 +' printNl.
(parser run: '1 * 2 + 3 * 4 + 5') printNl.
'result: 1 2 * 3 4 * 5 + +' printNl."
