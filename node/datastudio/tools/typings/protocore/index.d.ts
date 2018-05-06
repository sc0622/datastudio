// index.d.ts

export as namespace jcore;

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
    readonly objectType: Number;
    id: String;
    domain: String;
    name: String;
    mark: String;
    desc: String;
    readonly isPrivateMark: Boolean;
    readonly objectTypeString: String;
    readonly childCount: Number;

    resetData(): void;
    clearData(): void;
    domainOfType(objectType: Number): String;
}

export class Item extends Object {
    constructor(type: ItemType);
    readonly bufferSize: Number;
    readonly bufferOffset: Number;
    data: Number;
    readonly dataString: String;
    defaultValue: Number;
    readonly typeName: String;
    readonly codeName: String;
    stringType(type: ItemType): String;
    static create(id: String, type: ItemType): Item;
}

export class BitItem extends Item {
    testBit(offset: Number): Boolean;
    bitStart: Number;
    bitCount: Number;
    typeSize: Number;
    readonly mask: String;
    readonly specs: Map<Number, String>;
    addSpec(key: Number, spec: String): void;
    removeSpec(key: Number): void;
    clearSpec(): void;
    specAt(key: Number): String;
    nameAt(offset: Number): String;
    nameOf(spec: String): String;
    descAt(offset: Number): String;
}

export class CheckItem extends Item {
    readonly isValid: Boolean;
    checkType: CheckType;
    readonly checkTypeString: String;
    static checkTypeString(checkType: CheckType): String;
    static stringCheckType(str: String): CheckType;
    startPos: Number;
    endPos: Number;
    readonly checkLength: Number;
}

export class ComplexItem extends Item {
    readonly table: Table;
    updateSend(period: Boolean): void;
    updateRecv(): void;
    resetSend(): void;
    itemByMark(mark: String, deep: Boolean): Item;
    tableByMark(mark: String, deep: Boolean): Table;
    itemByDomain(domain: String, domainType: DomainType): Item;
    tableByDomain(domain: String, domainType: DomainType): Table;
}

export class CounterItem extends Item {
    counterType: CounterType;
    static counterTypeString(counterType: CounterType): String;
    static stringCounterType(str: String): CounterType;
    value: Number;
}

export class FrameItem extends Item {
    addTable(table: Table): void;
    removeTable(code: Number): void;
    clearTable(): void;
    replaceCode(oldCode: Number, newCode: Number): void;
    readonly tables: Map<Number, Table>;
    tableAt(code: Number): Table;
    setBufferSize(size: Number): void;
    sequenceCount: Number;
}

export class FrameCodeItem extends Item {
    frameCodeType: FrameCodeType;
    readonly frameCodeTypeString: String;
    static frameCodeTypeString(frameCodeType: FrameCodeType): String; 
    static stringFrameCodeType(str: String): FrameCodeType;
    frameId: String;
    frame: FrameItem;
    updateSend(period: Boolean): void;
    updateRecv(): void;
    resetSend(): void;
}

export class HeaderItem extends Item {
    value: Number;
}

export class Limit extends Object {
    minimum: Number;
    maximum: Number;
    leftInf: Boolean;
    rightInf: Boolean;
    toString(): String;
}

export class NumericItem extends Item {
    numericType: NumericType;
    readonly frameCodeTypeString: String;
    static numericTypeString(numericType: NumericType): String; 
    static stringNumericType(str: String): NumericType;
    readonly originalData: Number;
    scale: Number;
    decimals: Number;
    offset: Number;
    limit: Limit;
    unit: String;
    specs: Map<Number, String>;
    addSpec(key: Number, spec: String): void;
    specAt(key: Number): String;
    removeSpec(key: Number): void;
    clearSpec(): void;
    readonly dataRange: Array<Number>[2];
    readonly valueRange: Array<Number>[2];
    readonly outOfLimit: Boolean;
}

export class Table extends Object {
    readonly itemOffset: Number;
    readonly bufferSize: Number;
    readonly bufferOffset: Number;
    readonly items: Array<Item>;
    appendItem(item: Item): void;
    insertItem(index: Number, item: Item): void;
    removeItem(index: Number): void;
    removeItemById(id: String): void;
    removeItemByMark(mark: String): void;
    clearItem(): void;
    readonly itemCount: Number;
    itemAt(index: Number): Item;
    itemById(id: String): Item;
    itemByMark(mark: String, deep: Boolean): Item;
    tableByMark(mark: String, deep: Boolean): Table;
    itemByDomain(domain: String, domainType: DomainType): Item;
    tableByDomain(domain: String, domainType: DomainType): Table;
    readonly counterItem: CounterItem;
    readonly isCheckValid: Boolean;
    readonly checkItem: CheckItem;
    readonly frameCodes: Array<FrameCodeItem>;
    updateSend(period: Boolean): void;
    updateRecv(): void;
    resetSend(): void;
    readonly sequence: Number;
    readonly period: Number;
    readonly isFrameTable: Boolean;
    readonly typeName: String;
    readonly codeName: String;
}

export class System extends Object {
    readonly tables: Array<Table>;
    appendTable(vehicle: Table): void;
    insertTable(index: Number, vehicle: Table): void;
    removeTable(index: Number): void;
    removeTableByMark(mark: String): void;
    clearTable(): void;
    readonly tableCount: Number;
    tableAt(index: Number): Table;
    tableByMark(mark: String): Table;
}

export class Vehicle extends Object {
    readonly systems: Array<System>;
    appendSystem(vehicle: System): void;
    insertSystem(index: Number, vehicle: System): void;
    removeSystem(index: Number): void;
    removeSystemByMark(mark: String): void;
    clearSystem(): void;
    readonly systemCount: Number;
    systemAt(index: Number): System;
    systemByMark(mark: String): System;
}

export class Root extends Object {
    readonly vehicles: Array<Vehicle>;
    appendVehicle(vehicle: Vehicle): void;
    insertVehicle(index: Number, vehicle: Vehicle): void;
    removeVehicle(index: Number): void;
    removeVehicleByMark(mark: String): void;
    clearVehicle(): void;
    readonly vehicleCount: Number;
    vehicleAt(index: Number): Vehicle;
    vehicleByMark(mark: String): Vehicle;
}
