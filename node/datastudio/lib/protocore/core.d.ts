// core.d.ts

export as namespace protocore;

export enum DomainType {
    DomainInvalid = -1,
    DomainId,
    DomainName,
    DomainMark
}

export enum ObjectType {
    ObjectInvalid = -1,
    ObjectRoot,
    ObjectVehicle,
    ObjectSystem,
    ObjectTable,
    ObjectItem,
    ObjectLimit
}

export enum ItemType {
    ItemInvalid = -1,   /**< 无效 */
    ItemHead,           /**< 包头 */
    ItemCounter,        /**< 帧计数 */
    ItemCheck,          /**< 校验 */
    ItemFrameCode,      /**< 帧识别码 */
    ItemNumeric,        /**< 数值类型 */
    ItemBitMap,         /**< BITMAP */
    ItemBitValue,       /**< BITVALUE */
    ItemComplex,        /**< 复合数据 */
    ItemFrame,          /**< 多帧数据 */
    ItemTotal           /**< 类型总数 */
}

export enum CheckType {
    CheckInvalid = -1, /**< 无效值 */
    CheckNone,      /**< 无校验 */
    CheckSum8,      /**< 8位和校验 */
    CheckSum16,     /**< 16位和校验 */
    CheckCrc8,      /**< Crc8校验 */
    CheckCrc16,     /**< Crc16校验 */
    CheckXor8,      /**< Xor8校验 */
    CheckXor16,     /**< Xor16校验 */
    CheckTotal      /**< 类型总数 */
}

export enum CounterType {
    CounterInvalid = -1,
    CounterU8,
    CounterU16,
    CounterU32,
    CounterU64,
    CounterTotal
}

export enum FrameCodeType {
    FrameCodeInvalid = -1,
    FrameCodeU8,
    FrameCodeU16,
    FrameCodeU32,
    FrameCodeU64,
    FrameCodeTotal
}

export enum NumericType {
    NumericInvalid = -1,
    NumericInt8,
    NumericUint8,
    NumericInt16,
    NumericUint16,
    NumericInt32,
    NumericUint32,
    NumericInt64,
    NumericUint64,
    NumericFloat32,
    NumericFloat64,
    NumericTotal
}

export class Object {
    readonly objectType: number;
    id: string;
    domain: string;
    name: string;
    mark: string;
    desc: string;
    readonly isPrivateMark: boolean;
    readonly objectTypestring: string;
    readonly childCount: number;

    resetData(): void;
    clearData(): void;
    domainOfType(objectType: number): string;
}

export class Item extends Object {
    constructor(type: ItemType);
    readonly bufferSize: number;
    readonly bufferOffset: number;
    data: number;
    readonly datastring: string;
    defaultValue: number;
    readonly typeName: string;
    readonly codeName: string;
    stringType(type: ItemType): string;
    static create(id: string, type: ItemType): Item;
}

export class Bit extends Item {
    constructor();
    testBit(offset: number): boolean;
    bitStart: number;
    bitCount: number;
    typeSize: number;
    readonly mask: string;
    readonly specs: Map<number, string>;
    addSpec(key: number, spec: string): void;
    removeSpec(key: number): void;
    clearSpec(): void;
    specAt(key: number): string;
    nameAt(offset: number): string;
    nameOf(spec: string): string;
    descAt(offset: number): string;
}

export class Check extends Item {
    constructor();
    readonly isValid: boolean;
    checkType: CheckType;
    readonly checkTypestring: string;
    static checkTypestring(checkType: CheckType): string;
    static stringCheckType(str: string): CheckType;
    startPos: number;
    endPos: number;
    readonly checkLength: number;
}

export class Complex extends Item {
    constructor();
    readonly table: Table;
    updateSend(period: boolean): void;
    updateRecv(): void;
    resetSend(): void;
    itemByMark(mark: string, deep: boolean): Item;
    tableByMark(mark: string, deep: boolean): Table;
    itemByDomain(domain: string, domainType: DomainType): Item;
    tableByDomain(domain: string, domainType: DomainType): Table;
}

export class Counter extends Item {
    constructor();
    counterType: CounterType;
    static counterTypestring(counterType: CounterType): string;
    static stringCounterType(str: string): CounterType;
    value: number;
}

export class Frame extends Item {
    constructor();
    addTable(table: Table): void;
    removeTable(code: number): void;
    clearTable(): void;
    replaceCode(oldCode: number, newCode: number): void;
    readonly tables: Map<number, Table>;
    tableAt(code: number): Table;
    setBufferSize(size: number): void;
    sequenceCount: number;
}

export class FrameCode extends Item {
    constructor();
    frameCodeType: FrameCodeType;
    readonly frameCodeTypestring: string;
    static frameCodeTypestring(frameCodeType: FrameCodeType): string; 
    static stringFrameCodeType(str: string): FrameCodeType;
    frameId: string;
    frame: Frame;
    updateSend(period: boolean): void;
    updateRecv(): void;
    resetSend(): void;
}

export class Header extends Item {
    constructor();
    value: number;
}

export class Limit extends Object {
    constructor();
    minimum: number;
    maximum: number;
    leftInf: boolean;
    rightInf: boolean;
    tostring(): string;
}

export class Numeric extends Item {
    constructor();
    numericType: NumericType;
    readonly frameCodeTypestring: string;
    static numericTypestring(numericType: NumericType): string; 
    static stringNumericType(str: string): NumericType;
    readonly originalData: number;
    scale: number;
    decimals: number;
    offset: number;
    limit: Limit;
    unit: string;
    specs: Map<number, string>;
    addSpec(key: number, spec: string): void;
    specAt(key: number): string;
    removeSpec(key: number): void;
    clearSpec(): void;
    readonly dataRange: Array<number>[2];
    readonly valueRange: Array<number>[2];
    readonly outOfLimit: boolean;
}

export class Table extends Object {
    constructor();
    readonly itemOffset: number;
    readonly bufferSize: number;
    readonly bufferOffset: number;
    readonly items: Array<Item>;
    appendItem(item: Item): void;
    insertItem(index: number, item: Item): void;
    removeItem(index: number): void;
    removeItemById(id: string): void;
    removeItemByMark(mark: string): void;
    clearItem(): void;
    readonly itemCount: number;
    itemAt(index: number): Item;
    itemById(id: string): Item;
    itemByMark(mark: string, deep: boolean): Item;
    tableByMark(mark: string, deep: boolean): Table;
    itemByDomain(domain: string, domainType: DomainType): Item;
    tableByDomain(domain: string, domainType: DomainType): Table;
    readonly counter: Counter;
    readonly isCheckValid: boolean;
    readonly check: Check;
    readonly frameCodes: Array<FrameCode>;
    updateSend(period: boolean): void;
    updateRecv(): void;
    resetSend(): void;
    readonly sequence: number;
    readonly period: number;
    readonly isFrameTable: boolean;
    readonly typeName: string;
    readonly codeName: string;
}

export class System extends Object {
    constructor();
    readonly tables: Array<Table>;
    appendTable(vehicle: Table): void;
    insertTable(index: number, vehicle: Table): void;
    removeTable(index: number): void;
    removeTableByMark(mark: string): void;
    clearTable(): void;
    readonly tableCount: number;
    tableAt(index: number): Table;
    tableByMark(mark: string): Table;
}

export class Vehicle extends Object {
    constructor();
    readonly systems: Array<System>;
    appendSystem(vehicle: System): void;
    insertSystem(index: number, vehicle: System): void;
    removeSystem(index: number): void;
    removeSystemByMark(mark: string): void;
    clearSystem(): void;
    readonly systemCount: number;
    systemAt(index: number): System;
    systemByMark(mark: string): System;
}

export class Root extends Object {
    constructor();
    readonly vehicles: Array<Vehicle>;
    appendVehicle(vehicle: Vehicle): void;
    insertVehicle(index: number, vehicle: Vehicle): void;
    removeVehicle(index: number): void;
    removeVehicleByMark(mark: string): void;
    clearVehicle(): void;
    readonly vehicleCount: number;
    vehicleAt(index: number): Vehicle;
    vehicleByMark(mark: string): Vehicle;
}
