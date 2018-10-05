Object subclass: #BaseKeyValue.
BaseKeyValue instanceVariableNames: 'key value isVariable'.
BaseKeyValue extend [
    setKey: aKey withValue: aValue [
        key := aKey.
        value := aValue.
        isVariable := true.
        ^self
    ]
    setFunctionType [
        isVariable := false.
        ^self
    ]
    isKey: aKey [
        ^(key = aKey)
    ]
    value [^value]
    isVariable [^isVariable]
    asString [
        ^('%1 %2' % {key . value})
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
        node := BaseKeyValue new setKey: input withValue: (stack size + 1).
        stack add: node.
        ^(stack size)
    ]
    addFuntion: input [
        |node|
        node := BaseKeyValue new setKey: input withValue: (stack size + 1).
        node setFunctionType.
        stack add: node.
        ^(stack size)
    ]
    find: input [
        |node|
        node := (stack select: [:current | current isKey: input]) last.
        ^node
    ]
]
