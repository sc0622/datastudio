// parser.d.ts

/// <reference path="core.d.ts" />

import {Object, Root, Vehicle, System, Table, Item} from './core';

export as namespace protocore;

export class ParserArgs {
  // ['file', 'sql']
  sourceType: string;
  filePath?: string;  //
}

export class Parser {
  static create(args: ParserArgs): Parser;

  parse(domain: string, objectType: number, deep: number): boolean | Root | Vehicle[] | Vehicle | System[] | System | Table[] | Table | Item[] | Item | Object;
  parseRoot(deep: number): boolean | Root;
  parseVehicle(deep: number): boolean | Vehicle[];
  parseVehicle(vehicleId: string, deep: number): boolean | Vehicle;
  parseSystem(vehicleId: string, deep: number): boolean | System[];
  parseSystem(vehicleId: string, systemId: string, deep: number): boolean | System;
  parseTable(vehicleId: string, systemId: string, deep: number): boolean | Table[];
  parseTable(vehicleId: string, systemId: string, tableId: string, deep: number): boolean | Table;
  parseItem(vehicleId: string, systemId: string, tableId: string, deep: number): boolean | Item[];
  parseItem(vehicleId: string, systemId: string, tableId: string, itemId, deep: number): boolean | Item;
  parseTable(): boolean | Table[];
  parseTable(tableId: string): boolean | Table;

  save(root: Root): boolean;
  save(vehicles: Vehicle[]): boolean;
  save(vehicle: Vehicle, vehicleId: string): boolean;
  save(systems: System[], vehicleId: string): boolean;
  save(system: System, vehicleId: string, systemId: string): boolean;
  save(tables: Table[], vehicleId: string, systemId: string): boolean;
  save(tables: Table[], domain: string): boolean;
  save(table: Table, vehicleId: string, systemId: string, tableId: string): boolean;
  save(items: Item[], vehicleId: string, systemId: string, tableId: string): boolean;
  save(items: Item[], domain: string): boolean;
  save(item: Item, vehicleId: string, systemId: string, tableId: string, itemId: string): boolean;
  save(tables: Table[]): boolean;
  save(table: Table, tableId: string): boolean;
  save(table: Table): boolean;
  save(domain: string, object: Object, merge: boolean, fast: boolean): Object;

  beginModify(): boolean;
  commitModify(): boolean;
  cancelModify(): boolean;
  endModify(): boolean;
  isBeginModify(): boolean;

  saveAs(table: Table, filePath: string): boolean;

  message(): string;
  setMessage(message: string): void;

  progressValue(): number;
  setProgressValue(value: number);

  canceledSaveAs(): boolean;
  cancelSaveAs(cancel: boolean): void;

  static queryTable(filePath: string, vehicleId: string, systemId: string, tableId: string): boolean | string;
}
