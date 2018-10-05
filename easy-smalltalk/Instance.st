FileStream fileIn: 'ModifyOrderedCollection.st'.


Object subclass: #Instance.
Instance instanceVariableNames: 'type data code'.
Instance class extend [
    create: aTypeName withPropertySize: aSize [
        ^((super new) init: aTypeName withPropertySize: aSize)
    ]
    create: aTypeName withPropertySize: aSize andCode: aCode [
        ^((super new) init: aTypeName withPropertySize: aSize andCode: aCode)
    ]
]
Instance extend [
    init: aTypeName withPropertySize: aSize [
        type := aTypeName.
        data := ((Interval from: 1 to: aSize) collect: [:current | 0]) asOrderedCollection.
        code := nil.
        ^self
    ]
    init: aTypeName withPropertySize: aSize andCode: aCode [
        type := aTypeName.
        data := ((Interval from: 1 to: aSize) collect: [:current | 0]) asOrderedCollection.
        code := aCode.
        ^self
    ]
    at: aKey put: aValue [
        data at: aKey put: aValue.
        ^self
    ]
    at: aKey [
        ^(data at: aKey)
    ]
    code [
        ^code
    ]
    asString [
        ^('(%1 %2 (%3))' % {type . data size . code})
    ]
]
