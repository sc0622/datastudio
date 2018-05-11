// comm.d.ts

export as namespace protocore;

export enum ChannelType {
  ChannelInvalid = -1,  // 通信类型不可用
  ChannelSerial,        // 串口通信方式
  ChannelUdp,           // UDP通信方式
  ChannelFile,          // 文件方式
  ChannelTotal          // 标记边界
}

export abstract class Channel {
  static create(config: string): boolean|Channel;
  static create(type: ChannelType): boolean|Channel;
  open(): boolean;
  close(): boolean;
  waitForReadyRead(msecs: number): boolean;
  read(size: number): boolean|Int8Array;
  write(buffer: Int8Array): number;
  sizeOfIn(): number;
  sizeofOut(): number;
  flush(): void;
  isOpen(): boolean;
  readonly channelType: ChannelType;
  identity: string;
  config: string;
  name: string;
  desc: string;
  readonly lastErrorCode: number;
  readonly lastError: string;
  replayer: Channel;
  static parseConfig(config: string): boolean|Map<string, string>;
}

export namespace FileChannel {
  export enum OpenMode {
    NotOpen = 0x0000,
    ReadOnly = 0x0001,
    WriteOnly = 0x0002,
    ReadWrite = ReadOnly | WriteOnly,
    Append = 0x0004,
    Truncate = 0x0008,
    Text = 0x0010,
    Unbuffered = 0x0020
  }
  export enum SaveFormat {
    SaveFormatDomain = 0x01,     // 包含表域名
    SaveFormatTimestamp = 0x02,  // 包含时间戳
    SaveFormatNormal = SaveFormatDomain | SaveFormatTimestamp
  }
}

export class FileChannel extends Channel {
  constructor(filePath: string, openMode?: FileChannel.OpenMode);
  filePath: string;
  openMode: FileChannel.OpenMode;
  saveFormat: FileChannel.SaveFormat;
  fileHeader: number;
  domain: string;
}

export namespace SerialChannel {
  export enum DataBits {
    Data5 = 5,
    Data6 = 6,
    Data7 = 7,
    Data8 = 8,
    UnknownDataBits = -1
  }
  export enum StopBits {
    OneStop = 1,
    OneAndHalfStop = 3,
    TwoStop = 2,
    UnknownStopBits = -1
  }
  export enum Parity {
    NoParity = 0,
    EvenParity = 2,
    OddParity = 3,
    SpaceParity = 4,
    MarkParity = 5,
    UnknownParity = -1
  }
}

export class SerialChannel extends Channel {
  constructor(
      portName: string, baudRate: number, dataBits?: SerialChannel.DataBits,
      stopBits?: SerialChannel.StopBits, parity?: SerialChannel.Parity);
  setConfig(
      portName: string, baudRate: number, dataBits: SerialChannel.DataBits,
      stopBits: SerialChannel.StopBits, parity: SerialChannel.Parity): boolean;
  portName: string;
  baudRate: number;
  dataBits: SerialChannel.DataBits;
  stopBits: SerialChannel.StopBits;
  parity: SerialChannel.Parity;
}

export namespace UdpChannel {
  export enum OpenMode {
    NotOpen = 0x0000,
    ReadOnly = 0x0001,
    WriteOnly = 0x0002,
    ReadWrite = ReadOnly | WriteOnly
  }
}

export class UdpChannel extends Channel {
  constructor(
      localAddr: string, localPort: number, remoteAddr: string,
      remotePort: number, openMode?: UdpChannel.OpenMode);
  openMode: UdpChannel.OpenMode;
  setConfig(
      localAddr: string, localPort: number, remoteAddr: string,
      remotePort: number, openMode: UdpChannel.OpenMode): boolean;
  readonly isMulticastIP: boolean;
  localAddr: string;
  localPort: number;
  remoteAddr: string;
  remotePort: number;
}