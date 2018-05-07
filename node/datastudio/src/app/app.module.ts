import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { BrowserAnimationsModule } from "@angular/platform-browser/animations";
import { NgxElectronModule } from 'ngx-electron';
import { FormsModule } from "@angular/forms";

import { MatButtonModule, MatToolbarModule, MatInputModule, MatProgressSpinnerModule,
  MatCardModule, MatTabsModule, MatTableModule, MatTreeModule, MatIconModule} 
  from "@angular/material";
import { CdkTreeModule } from "@angular/cdk/tree";

import { HttpClientModule, HttpClient } from "@angular/common/http";
import { TranslateModule, TranslateLoader} from '@ngx-translate/core';
import { TranslateHttpLoader} from '@ngx-translate/http-loader';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { EditorComponent } from './editor/editor.component';
import { PreferComponent } from './prefer/prefer.component';
import { MonitorComponent } from './monitor/monitor.component';
import { SimulatorComponent } from './simulator/simulator.component';
import { AnalyseComponent } from './analyse/analyse.component';
import { AboutComponent } from './about/about.component';

export function createTranslateLoader(http: HttpClient) {
  return new TranslateHttpLoader(http, './assets/i18n/', '.json');
}

@NgModule({
  declarations: [
    AppComponent,
    EditorComponent,
    PreferComponent,
    MonitorComponent,
    SimulatorComponent,
    AnalyseComponent,
    AboutComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpClientModule,
    TranslateModule.forRoot({
      loader: {
          provide: TranslateLoader,
          useFactory: (createTranslateLoader),
          deps: [HttpClient]
      }
    }),
    AppRoutingModule,
    NgxElectronModule,
    BrowserAnimationsModule,
    // material
    MatButtonModule, MatToolbarModule, MatInputModule, MatProgressSpinnerModule,
    MatCardModule, MatTreeModule, MatTabsModule, MatTableModule, MatIconModule,
    // cdk
    CdkTreeModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
