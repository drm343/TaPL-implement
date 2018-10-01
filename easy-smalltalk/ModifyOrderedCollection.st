OrderedCollection class extend [
    copyFrom: aCollection withSize: aSize [
        |result|
        result := (Interval from: 1 to: (aSize - aCollection size)) asOrderedCollection collect: [:reset | 0].
        ^(aCollection , result)
    ]
]
OrderedCollection extend [
    printNl [
        |result|
        result := ''.
        self collect: [:next | result := '%1 %2' % {result . next asString}].
        result printNl
    ]
    reduceWith: function [
        |result interval|
        result := OrderedCollection new.
        interval := Interval from: 1 to: (self size).
        interval collect: [ :next |
            (next >= (self size))
                ifTrue: [
                    result add: (function value: (self at: next) value: nil)
                ]
                ifFalse: [
                    result add: (function value: (self at: next) value: (self at: (next + 1)))
                ]
        ].
        ^result
    ]
]
