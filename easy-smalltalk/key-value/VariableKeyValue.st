FileStream fileIn: 'key-value/BaseKeyValue.st'.


BaseKeyValue subclass: #VariableKeyValue.
VariableKeyValue instanceVariableNames: 'isVariable'.
VariableKeyValue extend [
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
    isVariable [^isVariable]
]
