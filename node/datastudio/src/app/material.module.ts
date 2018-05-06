import { NgModule } from "@angular/core";
import { CommonModule } from "@angular/common";
import { MatButtonModule, MatToolbarModule, MatInputModule, MatProgressSpinnerModule,
    MatCardModule, MatTabsModule, MatTableModule, MatTreeModule} 
    from "@angular/material";

@NgModule({
    imports: [
        MatButtonModule, MatToolbarModule, MatInputModule, MatProgressSpinnerModule,
        MatCardModule, MatTreeModule, MatTabsModule, MatTableModule
    ],
    exports: [
        MatButtonModule, MatToolbarModule, MatInputModule, MatProgressSpinnerModule,
        MatCardModule, MatTreeModule, MatTabsModule, MatTableModule
    ]
})

export class MaterialModule {
     
}