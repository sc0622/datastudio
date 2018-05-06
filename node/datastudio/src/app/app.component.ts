import {Component} from '@angular/core';
import {TranslateService} from '@ngx-translate/core';
import {ElectronService} from 'ngx-electron';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})

export class AppComponent {
  constructor(
      private electronService_: ElectronService,
      public translate: TranslateService) {
    translate.setDefaultLang('zh-CN');
    //
  }

  switchLanguage(language: string) {
    this.translate.use(language);
  }
}
