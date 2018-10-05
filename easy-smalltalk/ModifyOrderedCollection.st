OrderedCollection extend [
    printNl [
        |result|
        result := ''.
        self collect: [:next | result := '%1 %2' % {result . next asString}].
        result printNl
    ]
    collectWithNext: function [
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
    collectWithNext2Item: function [
        |result interval|
        result := OrderedCollection new.
        interval := Interval from: 1 to: (self size).
        interval collect: [ :next |
            (next >= (self size - 1))
                ifTrue: [
                    (next >= (self size))
                        ifTrue: [
                            result add: (function value: (self at: next) value: nil value: nil)
                        ]
                        ifFalse: [
                            result add: (function value: (self at: next) value: (self at: (next + 1)) value: nil)
                        ]
                ]
                ifFalse: [
                    result add: (function value: (self at: next) value: (self at: (next + 1)) value: (self at: (next + 2)))
                ]
        ].
        ^result
    ]
    resetSize: aSize [
        |result|
        result := (Interval from: 1 to: (aSize - self size)) asOrderedCollection collect: [:reset | 0].
        ^(self , result)
    ]
]
