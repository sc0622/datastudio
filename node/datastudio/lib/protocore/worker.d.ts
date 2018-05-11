// worker.d.ts

/// <reference path='comm.d.ts' />
/// <reference path='core.d.ts' />

import { Table } from './core';
import { Channel } from './comm';

export as namespace protocore;

export class Util {
  calcCrc16(buffer: Int8Array): number;
  doCheck(table: Table, buffer: Int8Array): boolean;
  // checkData(table: Table, fileHeaderSize: number, source: Object, target: Object): boolean;
}

export namespace Worker {
    export enum WorkerType {
        WorkerTypeSend,
        WorkerTypeRecv
    }
}

export class Worker {
  constructor(channel: Channel, tableSend?: Table, tableRecv?: Table);
  channel: Channel;
  repaler: Worker;
  readonly workerSend: WorkerSend;
  readonly workerRecv: WorkerRecv;
  readonly isOpen: boolean;
  open(): boolean;
  close(): void;
}

export class WorkerTrans {

}

export class WorkerSend extends WorkerTrans {

}

export class WorkerRecv extends WorkerTrans {

}

export class WorkerPool {

}
