
export as namespace jx;

export interface Test1 {
    test1(): void;
}

export class Test2 implements Test1 {
    test1(): void;
}