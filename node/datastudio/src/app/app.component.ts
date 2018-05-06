import {Component} from '@angular/core';
import {ElectronService} from 'ngx-electron';
import { jcore } from "../../lib/protocore"

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html', 
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  constructor(private electronService_: ElectronService) {
    var numeric = new jcore.Numeric();
    console.log(numeric.mark);
  }   
 
  answer: String = '';
  answerDisplay: String = ''; 
  showSpinner: Boolean = false;  

  numeric: jcore.Numeric = new jcore.Numeric();

  showAnswer() { 
    this.showSpinner = true;
    this.answerDisplay = ''; 
    this.numeric.mark = 'helloworld';

    setTimeout(() => {
      this.answerDisplay = this.numeric.mark + this.answer;
      this.showSpinner = false;
    }, 2000);
  }
}
