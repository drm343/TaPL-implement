Object subclass: #BaseNode.
BaseNode instanceVariableNames: 'value type priority'.
BaseNode class extend [
    set: input [
        |yourself|
        yourself := super new init.
        ^(yourself set: input)
    ]
]
BaseNode extend [
    init [
        type := nil.
        priority := 0.
        ^self
    ]
    set: input [
        value := input.
        ^self
    ]
    print [
        ('(%1 %2) ' % {type . value}) print
    ]
    printNl [
        ('(%1 %2) ' % {type . value}) printNl
    ]
    asString [
        ^('(%1 %2) ' % {type . value})
    ]
    priority [^priority]
    value [^value]
    isSmallerPriority: object [
        ^(priority < (object priority))
    ]
    isEqualPriority: object [
        ^(priority = (object priority))
    ]
    isBiggerPriority: object [
        ^(priority > (object priority))
    ]
    addPriority: value [
        priority := priority + value.
        ^self
    ]
]


BaseNode subclass: #NodeNumber.
NodeNumber class extend [
    print [
        'is number' printNl
    ]
]
NodeNumber extend [
    init [
        super init.
        type := 'number'.
        ^self
    ]
]


BaseNode subclass: #NodeOp.
NodeOp class extend [
    print [
        'is op' printNl
    ]
]
NodeOp extend [
    init [
        super init.
        type := 'op'.
        ^self
    ]
    set: input [
        super set: input.
        (value = '+')
            ifTrue: [priority := 1].
        (value = '-')
            ifTrue: [priority := 1].
        (value = '*')
            ifTrue: [priority := 1].
        (value = '/')
            ifTrue: [priority := 1].
        ^self
    ]
]


BaseNode subclass: #NodeBracket.
NodeBracket class extend [
    print [
        'is bracket' printNl
    ]
]
NodeBracket extend [
    init [
        super init.
        priority := 10.
        type := 'bracket'.
        ^self
    ]
]


BaseNode subclass: #NodeExpression.
NodeExpression extend [
    init [
        super init.
priority := 0.
        type := 'expression-end'.
        ^self
    ]
]
