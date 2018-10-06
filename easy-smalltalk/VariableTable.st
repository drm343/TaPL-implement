FileStream fileIn: 'key-value/VariableKeyValue.st'.


Object subclass: #VariableTable.
VariableTable instanceVariableNames: 'stack'.
VariableTable class extend [
    create [
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
        node := VariableKeyValue new setKey: input withValue: (stack size + 1).
        stack add: node.
        ^(stack size)
    ]
    addFuntion: input [
        |node|
        node := VariableKeyValue new setKey: input withValue: (stack size + 1).
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
