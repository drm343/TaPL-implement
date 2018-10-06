FileStream fileIn: 'key-value/BaseKeyValue.st'.


Object subclass: #MessageTable.
MessageTable instanceVariableNames: 'stack'.
MessageTable class extend [
    create [
        ^(super new init)
    ]
]
MessageTable extend [
    init [
        stack := OrderedCollection new.
        ^self
    ]
    add: value [
        |node|
        node := BaseKeyValue new setKey: value withValue: (stack size + 1).
        stack add: node.
        ^(stack size)
    ]
    findOrCreateBy: value [
        |node|
        node := stack select: [:current | current isKey: value].
        (node size >= 1)
            ifTrue: [^(node last value)]
            ifFalse: [^(self add: value)]
    ]
]
