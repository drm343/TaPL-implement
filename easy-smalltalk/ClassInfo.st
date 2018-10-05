FileStream fileIn: 'VariableTable.st'.


Object subclass: #ClassInfo.
ClassInfo instanceVariableNames: 'type variableTable methodTable'.
ClassInfo class extend [
    init: name [
        ^(super new init: name)
    ]
]
ClassInfo extend [
    init: name [
        variableTable := OrderedCollection new.
        methodTable := OrderedCollection new.
        type := name.
        ^self
    ]
    type [ ^type ]
    propertySize [ ^(variableTable size) ]
    isName: input [
        ^(type = input)
    ]
    addVariable: input [
        |node|
        node := BaseKeyValue new setKey: input withValue: (variableTable size + 1).
        variableTable add: node.
        ^(variableTable size)
    ]
    addMethod: input [
        |node|
        node := BaseKeyValue new setKey: input withValue: (methodTable size + 1).
        node setFunctionType.
        methodTable add: node.
        ^(methodTable size)
    ]
    findVariable: input [
        |result|
        result := variableTable select: [:current | current isKey: input].
        (result size <= 0)
            ifTrue: [^nil]
            ifFalse: [^(result last)]
    ]
    findMethod: input [
        |node|
        node := (methodTable select: [:current | current isKey: input]) last.
        ^node
    ]
    asString [
        ^('(classTable %1 %2 %3)' % {type . variableTable . methodTable})
    ]
]
