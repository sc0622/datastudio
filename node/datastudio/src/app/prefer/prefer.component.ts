import {NestedTreeControl} from '@angular/cdk/tree';
import {Component, Injectable, OnInit} from '@angular/core';
import {MatTreeNestedDataSource} from '@angular/material/tree';
import {BehaviorSubject, of as observableOf} from 'rxjs';

import * as jx from '../../../lib/protocore';

/**
 * Json node data with nested structure. Each node has a filename and a value or
 * a list of children 
 */
export class FileNode {
  children: FileNode[];
  filename: string;
  type: any;
}

/**
 * The Json tree data in string. The data could be parsed into Json object
 */
const TREE_DATA = `
{
    "Documents": {
      "angular": {
        "src": {
          "core": "ts",
          "compiler": "ts"
        }
      },
      "material2": {
        "src": {
          "button": "ts",
          "checkbox": "ts",
          "input": "ts"
        }
      }
    },
    "Downloads": {
        "Tutorial": "html",
        "November": "pdf",
        "October": "pdf"
    },
    "Pictures": {
        "Sun": "png",
        "Woods": "jpg",
        "Photo Booth Library": {
          "Contents": "dir",
          "Pictures": "dir"
        }
    },
    "Applications": {
        "Chrome": "app",
        "Calendar": "app",
        "Webstorm": "app"
    }
}`;

@Injectable()
export class FileDatabase {
  dataChange: BehaviorSubject<FileNode[]> = new BehaviorSubject<FileNode[]>([]);

  get data(): FileNode[] {
    return this.dataChange.value;
  }

  constructor() {
    this.initialize();
  }

  initialize() {
    // Parse the string to json object.
    const dataObject = JSON.parse(TREE_DATA);

    // Build the tree nodes from Json object. The result is a list of `FileNode`
    // with nested
    //     file node as children.
    const data = this.buildFileTree(dataObject, 0);

    // Notify the change.
    this.dataChange.next(data);
  }

  /**
   * Build the file structure tree. The `value` is the Json object, or a
   * sub-tree of a Json object. The return value is the list of `FileNode`.
   */
  buildFileTree(value: any, level: number): FileNode[] {
    let data: any[] = [];
    for (let k in value) {
      let v = value[k];
      let node = new FileNode();
      node.filename = `${k}`;
      if (v === null || v === undefined) {
        // no action
      } else if (typeof v === 'object') {
        node.children = this.buildFileTree(v, level + 1);
      } else {
        node.type = v;
      }
      data.push(node);
    }
    return data;
  }
}

@Component({
  selector: 'app-prefer',
  templateUrl: './prefer.component.html',
  styleUrls: ['./prefer.component.scss'],
  providers: [FileDatabase]
})
export class PreferComponent implements OnInit {
  answer: string = '';
  answerDisplay: string = '';
  showSpinner: boolean = false;

  numeric: jx.Numeric = new jx.Numeric();

  nestedTreeControl: NestedTreeControl<FileNode>; 
  nestedDataSource: MatTreeNestedDataSource<FileNode>;
  private _getChildren = (node: FileNode) => {
    return observableOf(node.children);
  };
  hasNestedChild = (_: number, nodeData: FileNode) => {
    return !(nodeData.type);
  };

  constructor(private database_: FileDatabase) {
    //
    this.nestedTreeControl = new NestedTreeControl<FileNode>(this._getChildren);
    this.nestedDataSource = new MatTreeNestedDataSource();
    database_.dataChange.subscribe(data => this.nestedDataSource.data = data);
  }

  ngOnInit() {}

  showAnswer() {
    this.showSpinner = true;
    this.answerDisplay = '';
    //this.numeric.mark = 'helloworld';

    setTimeout(() => {
      this.answerDisplay = this.numeric.mark + this.answer;
      this.showSpinner = false;
    }, 2000);
  }
}
