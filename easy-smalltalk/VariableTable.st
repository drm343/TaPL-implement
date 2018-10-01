Object subclass: #Variable.
Variable instanceVariableNames: 'key value'.
Variable extend [
    setKey: aKey withValue: aValue [
        key := aKey.
        value := aValue.
        ^self
    ]
    isKey: aKey [
        ^(key = aKey)
    ]
    value [^value]
    printNl [
        ('%1 %2' % {key . value}) printNl
    ]
]


Object subclass: #VariableTable.
VariableTable instanceVariableNames: 'stack'.
VariableTable class extend [
    init [
        ^(super new init)
    ]
]
VariableTable extend [
    init [
        stack := OrderedCollection new.
        ^self
    ]
    add: input [
        |node|
        node := Variable new setKey: input withValue: (stack size + 1).
        stack add: node.
        ^(stack size)
    ]
    find: input [
        |node|
        node := (stack select: [:current | current isKey: input]) last.
        ^(node value)
    ]
]
