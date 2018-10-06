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
    asString [
        ^('%1 %2' % {key . value})
    ]
]
