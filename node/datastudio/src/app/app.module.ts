import { BrowserModule,  } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';

import { BrowserAnimationsModule } from "@angular/platform-browser/animations";
import { NgxElectronModule } from 'ngx-electron';
import { MaterialModule } from "./material.module";
import { CdkModule } from "./cdk.module";
import { FormsModule } from "@angular/forms";

@NgModule({
  declarations: [
    AppComponent
  ],
  imports: [
    BrowserModule, 
    NgxElectronModule,
    BrowserAnimationsModule,
    MaterialModule,
    CdkModule,
    FormsModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
