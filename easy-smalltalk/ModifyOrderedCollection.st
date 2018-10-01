OrderedCollection class extend [
]
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
    resetSize: aSize [
        |result|
        result := (Interval from: 1 to: (aSize - self size)) asOrderedCollection collect: [:reset | 0].
        ^(self , result)
    ]
]
