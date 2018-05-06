import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { PreferComponent } from "./prefer/prefer.component";
import { EditorComponent } from "./editor/editor.component";
import { MonitorComponent } from "./monitor/monitor.component";
import { SimulatorComponent } from "./simulator/simulator.component";
import { AnalyseComponent } from "./analyse/analyse.component";
import { AboutComponent } from "./about/about.component";

const routes: Routes = [
  { path: 'prefer', component: PreferComponent },
  { path: 'editor', component: EditorComponent },
  { path: 'monitor', component: MonitorComponent },
  { path: 'simulator', component: SimulatorComponent },
  { path: 'analyse', component: AnalyseComponent },
  { path: 'about', component: AboutComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
