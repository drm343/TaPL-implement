Object subclass: #BaseKeyValue.
BaseKeyValue instanceVariableNames: 'key value'.
BaseKeyValue extend [
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


Object subclass: #BaseTable.
BaseTable instanceVariableNames: 'stack'.
BaseTable class extend [
    init [
        ^(super new init)
    ]
]
BaseTable extend [
    init [
        stack := OrderedCollection new.
        ^self
    ]
    add: input [
        |node|
        node := BaseKeyValue new setKey: input withValue: (stack size + 1).
        stack add: node.
        ^(stack size)
    ]
    find: input [
        |node|
        node := (stack select: [:current | current isKey: input]) last.
        ^(node value)
    ]
]
