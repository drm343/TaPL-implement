OrderedCollection extend [
    printNl [
        |result|
        result := ''.
        self collect: [:next | result := '%1 %2' % {result . next asString}].
        result printNl
    ]
]
