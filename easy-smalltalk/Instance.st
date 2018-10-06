FileStream fileIn: 'ModifyOrderedCollection.st'.


Object subclass: #Instance.
Instance instanceVariableNames: 'type data code classInfo'.
Instance class extend [
    createWithInfo: aInfo andCode: aCode [
        ^((super new) initWithInfo: aInfo andCode: aCode)
    ]
    create: aTypeName withPropertySize: aSize [
        ^((super new) init: aTypeName withPropertySize: aSize)
    ]
    create: aTypeName withPropertySize: aSize andCode: aCode [
        ^((super new) init: aTypeName withPropertySize: aSize andCode: aCode)
    ]
]
Instance extend [
    initWithInfo: aInfo andCode: aCode [
        type := aInfo type.
        data := ((Interval from: 1 to: (aInfo propertySize)) collect: [:current | 0]) asOrderedCollection.
        code := OrderedCollection new.
        code add: aCode.
        classInfo := aInfo.
        ^self
    ]
    init: aTypeName withPropertySize: aSize [
        type := aTypeName.
        data := ((Interval from: 1 to: aSize) collect: [:current | 0]) asOrderedCollection.
        code := nil.
        ^self
    ]
    init: aTypeName withPropertySize: aSize andCode: aCode [
        type := aTypeName.
        data := ((Interval from: 1 to: aSize) collect: [:current | 0]) asOrderedCollection.
        code := OrderedCollection new.
        code add: aCode.
        ^self
    ]
    at: aKey put: aValue [
        data at: aKey put: aValue.
        ^self
    ]
    at: aKey [
        ^(data at: aKey)
    ]
    loadCode: aKey [
        |aPosition|
        aPosition := classInfo findMethod: aKey.
        ^(code at: aPosition)
    ]
    asString [
        ^('(%1 %2 (%3))' % {type . data size . code})
    ]
]
