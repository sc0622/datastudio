import { Component } from '@angular/core';
import { ElectronService } from 'ngx-electron'

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
/*
  constructor(private _electronService: ElectronService) {}

  launchWindow() {
    this._electronService.shell.openExternal('https:/www.baidu.com');
  }
*/

  answer: String = '';
  answerDisplay: String = '';
  showSpinner: Boolean = false;

  showAnswer() {
    this.showSpinner = true;
    this.answerDisplay = '';

    setTimeout(() => {
      this.answerDisplay = this.answer;
      this.showSpinner = false;
    }, 2000);
  }
}
